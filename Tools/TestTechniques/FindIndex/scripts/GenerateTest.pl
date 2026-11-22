#!/usr/bin/env perl
use strict;
use warnings;
use File::Path qw(make_path);
use File::Copy qw(copy);
use Getopt::Long qw(GetOptions);
use HTTP::Tiny;

my $SCHEMA_URL = 'https://raw.githubusercontent.com/repmyblock/utilities/refs/heads/master/Schemas/2026_10_14_RepMyBlock.sql';
my ($root, $schema_file, $force, $help);
GetOptions('root=s' => \$root, 'schema=s' => \$schema_file,
           'force!' => \$force, 'help' => \$help) or usage(2);
usage(0) if $help;
$root //= shift @ARGV;
usage(2) unless defined($root) && !@ARGV;
$root =~ s{/$}{} unless $root eq '/';
my $module_dir = "$root/../datamodules";
make_path($module_dir) unless -d $module_dir;

my $sql;
if (defined $schema_file) {
  open my $fh, '<:raw', $schema_file or die "Cannot read schema '$schema_file': $!\n";
  local $/; 
  $sql = <$fh>; 
  close $fh;
} else {
  my $response = HTTP::Tiny->new(timeout => 30)->get($SCHEMA_URL);
  die "Could not download schema: $response->{status} $response->{reason}\n"
    unless $response->{success};
  $sql = $response->{content};
}

my @tables = parse_schema($sql);
die "No Data*/Voter* tables found in the schema.\n" unless @tables;

my ($written, $skipped) = (0, 0);
for my $t (@tables) {
  my ($module, $type) = module_names($t->{table});
  my $base = "$module$type";
  my %files = (
    "$module_dir/$base.h" => render_header($module, $type, $t),
    "$module_dir/$base.c" => render_source($module, $type, $t),
    "$root/Check$base.c"  => render_check($module, $type, $t),
    # Keep the Module-suffixed checker name used by the FindIndex Makefile.
    "$root/Check${base}Module.c" => render_check($module, $type, $t),
  );
  for my $path (sort keys %files) {
    if (-e $path && !$force) {
      print "SKIP existing $path (use --force to replace)\n";
      $skipped++;
      next;
    }
    if (-e $path && $force) {
      my $backup = "$path.pre-GenerateTest.bak";
      copy($path, $backup) or die "Cannot back up '$path' to '$backup': $!\n"
          unless -e $backup;
    }
    open my $fh, '>:raw', $path or die "Cannot write '$path': $!\n";
    print {$fh} $files{$path} or die "Cannot write '$path': $!\n";
    close $fh or die "Cannot close '$path': $!\n";
    print "WROTE $path\n";
    $written++;
  }
}
print "Generated $written file(s); skipped $skipped existing file(s).\n";
update_makefile($root, \@tables) if -f "$root/Makefile";

sub usage {
  my ($code) = @_;
  print STDERR <<'USAGE';
Usage: ./GenerateTest.pl --root <PATH> [--schema <schema.sql>] [--force]

Reads Data* and Voter(s)* tables from a MySQL schema and generates each
datamodules/Data<Type>.c/.h or datamodules/Voter<Type>.c/.h, plus a matching
CheckData<Type>.c or CheckVoter<Type>.c. Generated modules include loaders,
RecordHash wrappers, query and parse functions, and a checker program.

When <PATH>/Makefile exists, it is updated to build a Check<Type>Module
executable for every generated module and to clean their object/dependency files.
Existing outputs are skipped by default. --force replaces each output after
creating a .pre-GenerateTest.bak copy if one does not already exist.
The Makefile is also backed up before the generator changes it.
USAGE
	exit $code;
}

sub update_makefile {
  my ($root, $tables) = @_;
  my $path = "$root/Makefile";
  open my $in, '<:raw', $path or die "Cannot read '$path': $!\n";
  local $/; my $make = <$in>; close $in;

  # A wildcard-based Makefile already discovers every checker, including
  # both Check<Type>.c and Check<Type>Module.c. Do not replace that logic
  # with the schema-specific target list below.
  if ($make =~ /^CHECK_SOURCES\s*=\s*\$\(wildcard Check\*\.c\)\s*$/m &&
      $make =~ /^CHECK_TARGETS\s*=\s*\$\(CHECK_SOURCES:\.c=\)\s*$/m) {
      return;
  }

  my @modules;
  for my $table (@$tables) {
      my ($module, $type) = module_names($table->{table});
      push @modules, "$module$type";
  }
  my $module_list = join(' ', @modules);
  my ($targets) = $make =~ /^TARGETS\s*=\s*(.*?)\s*$/m;
  die "Cannot update '$path': TARGETS assignment was not found\n" unless defined $targets;
  my @base_targets = grep { $_ ne '$(CHECK_TARGETS)' && $_ !~ /^Check(?:Data|Voter).*Module$/ }
                     split /\s+/, $targets;
  my $new_targets = 'TARGETS = ' . join(' ', @base_targets, '$(CHECK_TARGETS)');
  $make =~ s/^TARGETS\s*=.*$/$new_targets/m;

  if ($make =~ /^CHECK_MODULES\s*=/m) {
    $make =~ s/^CHECK_MODULES\s*=.*$/CHECK_MODULES = $module_list/m;
  } else {
    $make =~ s/^(OBJECTS\s*=)/CHECK_MODULES = $module_list\nCHECK_TARGETS = \$(addprefix Check,\$(addsuffix Module,\$(CHECK_MODULES)))\nCHECK_OBJECTS = \$(addsuffix .o,\$(CHECK_TARGETS))\n\n$1/m
          or die "Cannot update '$path': OBJECTS assignment was not found\n";
  }
  if ($make !~ /^CHECK_TARGETS\s*=/m) {
    $make =~ s/^(CHECK_MODULES\s*=.*)$/\$1\nCHECK_TARGETS = \$(addprefix Check,\$(addsuffix Module,\$(CHECK_MODULES)))\nCHECK_OBJECTS = \$(addsuffix .o,\$(CHECK_TARGETS))/m;
  }
  if ($make =~ /^OBJECTS\s*=.*$/m) {
    $make =~ s/^(OBJECTS\s*=.*)$/index($1, '$(CHECK_OBJECTS)') >= 0 ? $1 : "$1 \$(CHECK_OBJECTS)"/me;
  }

  my $rules = <<'MAKE';
define CHECK_MODULE_template
Check$(1)Module: $(COMMON_OBJECTS) Check$(1)Module.o datamodules/$(1).o
  $$(CC) $$(LDFLAGS) -o $$@ $$^ $$(LDLIBS)
endef
$(foreach module,$(CHECK_MODULES),$(eval $(call CHECK_MODULE_template,$(module))))

MAKE
  if ($make =~ /^CheckDataAddressModule:.*?^%.o:/ms) {
    $make =~ s/^CheckDataAddressModule:.*?^(?=%.o:)/$rules/ms;
  } elsif ($make !~ /^define CHECK_MODULE_template$/m) {
    die "Cannot update '$path': expected checker rules or generated checker template were not found\n";
  }

  return if $make eq do { local $/; open my $fh, '<:raw', $path or die "Cannot reread '$path': $!\n"; <$fh> };
  my $backup = "$path.pre-GenerateTest.bak";
  copy($path, $backup) or die "Cannot back up '$path' to '$backup': $!\n" unless -e $backup;
  open my $out, '>:raw', $path or die "Cannot write '$path': $!\n";
  print {$out} $make or die "Cannot write '$path': $!\n";
  close $out or die "Cannot close '$path': $!\n";
  print "UPDATED $path with checker targets for all generated modules\n";
}

sub parse_schema {
  my ($text) = @_;
  my @out;
  while ($text =~ /^CREATE TABLE `([^`]+)` \(\s*\n(.*?)^\)\s*ENGINE=/msg) {
    my ($table, $body) = ($1, $2);
    next unless $table =~ /\A(?:Data|Voters?)/;
    my @columns;
    for my $line (split /\n/, $body) {
      next unless $line =~ /^\s+`([^`]+)`\s+(.+?)(?:,)?\s*$/;
      my ($name, $def) = ($1, $2);
      next if $name =~ /\A(?:PRIMARY|UNIQUE|KEY|CONSTRAINT|FULLTEXT|SPATIAL)\z/;
      my ($ctype, $capacity, $text) = map_column($def, $table, $name);
      push @columns, { name=>$name, def=>$def, ctype=>$ctype,
                       capacity=>$capacity, text=>$text };
    }
    next unless @columns;
    my ($pk) = $body =~ /^\s+PRIMARY KEY \(`([^`]+)`\)/m;
    $pk //= $columns[0]{name};
    die "$table: primary key '$pk' must be the first selected column\n"
        unless $columns[0]{name} eq $pk;
    die "$table: RecordHash/RecordLoader support at most 31 searchable fields\n"
        if @columns - 1 > 31;
    push @out, { table=>$table, columns=>\@columns, pk=>$pk };
  }
  return @out;
}

sub map_column {
  my ($def, $table, $name) = @_;
  my $lower = lc $def;
  # RecordLoader accepts uint32_t and decimal strings. Signed or 64-bit
  # integer values are stored as text so they are never truncated/rejected.
  if ($lower =~ /\A(?:tinyint|smallint|mediumint|int|integer)\b/ &&
      $lower =~ /\bunsigned\b/) {
      return ('uint32_t', undef, 0);
  }
  my $size;
  if ($lower =~ /\A(?:tinyint|smallint|mediumint|int|integer|bigint)\b/) {
    $size = $lower =~ /\bbigint\b/ ? 32 : 16;
  }
  elsif ($lower =~ /\A(?:var)?char\((\d+)\)/) { $size = $1 * 4 + 1 }
  elsif ($lower =~ /\Aenum\((.*)\)/) {
    my ($max, $enum) = (1, $1);
    while ($enum =~ /'((?:\\.|[^'])*)'/g) {
      my $n = length($1) * 4 + 1; $max = $n if $n > $max;
    }
    $size = $max;
  }
  elsif ($lower =~ /\A(?:tinytext|text|mediumtext|longtext)\b/) { $size = 4097 }
  elsif ($lower =~ /\Adate\b/) { $size = 16 }
  elsif ($lower =~ /\A(?:time|year)\b/) { $size = 32 }
  elsif ($lower =~ /\A(?:datetime|timestamp)\b/) { $size = 40 }
  elsif ($lower =~ /\A(?:decimal|numeric|float|double|real)\b/) { $size = 128 }
  elsif ($lower =~ /\A(?:tinyblob|blob|mediumblob|longblob|binary|varbinary)\b/) {
     die "$table.$name has binary SQL type '$def'; a generated text hash cannot safely represent binary values\n";
  }
  else { die "Unsupported SQL type for $table.$name: $def\n" }
  return ('char', $size, 1);
}

sub module_names {
  my ($table) = @_;
  return ('Data', $1) if $table =~ /\AData(.+)\z/;
  if ($table =~ /\AVoters(.*)\z/) {
    my $suffix = $1;
    $suffix =~ s/Indexes\z/Index/;
    return ('Voter', $suffix);
  }
  return ('Voter', $1) if $table =~ /\AVoter(.+)\z/;
  die "Cannot map table name '$table' to module name\n";
}

sub member_name {
  my ($table, $column) = @_;
  my $name = $column;
  $name =~ s/^\Q$table\E_//;
  $name =~ s/[^A-Za-z0-9_]/_/g;
  my %reserved = map { $_ => 1 } qw(auto break case char const continue default do double else enum extern float for goto if inline int long register restrict return short signed sizeof static struct switch typedef union unsigned void volatile while _Alignas _Alignof _Atomic _Bool _Complex _Generic _Imaginary _Noreturn _Static_assert _Thread_local);
  $name .= '_field' if $reserved{$name};
  return $name;
}

sub api_name {
  my ($module, $type) = @_;
  return $type eq '' ? 'Voter' : "Voter$type" if $module eq 'Voter';
  return $type;
}

sub loader_name {
  my ($module, $type) = @_;
  return ($module eq 'Data' && $type eq 'Address') ? 'Addresses' : api_name($module, $type);
}

sub hash_columns {
  my ($module, $type, $t) = @_;
  # The address checker matches its nine user-supplied key fields. The
  # coordinate and OSM columns remain loaded/printed but are not key fields.
  my @cols = @{$t->{columns}}[1 .. $#{$t->{columns}}];
  if ($module eq 'Data' && $type eq 'Address') {
    @cols = grep { $_->{name} ne 'Cordinate_ID' && $_->{name} ne 'PG_OSM_osmid' } @cols;
  }
  return @cols;
}

sub render_header {
  my ($module, $type, $t) = @_;
  my $base = "$module$type";
  my $api = api_name($module, $type);
  my $loader = loader_name($module, $type);
  my $constant = uc($api);
  my $guard = uc("${base}_H");
  my $s = "#ifndef $guard\n";
  $s .= "#define $guard\n\n";
  $s .= "#include <stddef.h>\n";
  $s .= "#include <stdint.h>\n";
  $s .= "#include <mysql/mysql.h>\n";
  $s .= "#include \"../RecordHash.h\"\n\n";
  $s .= "typedef struct {\n";
  $s .= "  uint32_t ID;\n";  
  $s .= "  /* RecordLoader writes a uint32_t null mask at this offset. */\n";    
  $s .= "  uint32_t nullFields;\n";
    
  for my $col (@{$t->{columns}}[1 .. $#{$t->{columns}}]) {
    my $m = member_name($t->{table}, $col->{name});
    $s .= $col->{text} ? "  char $m\[$col->{capacity}\];\n" : "  uint32_t $m;\n";
  }
  
  $s .= "} $base;\n\n";
  $s .= "typedef struct ${api}HashTable ${api}HashTable;\n";
  $s .= "typedef RecordMatch ${api}Match;\n";
  $s .= "typedef RecordLookupResult ${api}LookupResult;\n";  
  $s .= "#define ${constant}_ERROR RECORD_ERROR\n";
  $s .= "#define ${constant}_NOT_FOUND RECORD_NOT_FOUND\n";
  $s .= "#define ${constant}_FOUND RECORD_FOUND\n";
  $s .= "#define ${constant}_DUPLICATE RECORD_DUPLICATE\n\n";
  $s .= "int Load${loader}IntoMemory3(MYSQL *, $base **, size_t *);\n";
  $s .= "int Load${loader}IntoMemory4(MYSQL *, $base **, size_t *, size_t);\n";
  $s .= "#define ${loader}_LOAD_SELECT(_1,_2,_3,_4,NAME,...) NAME\n";
  $s .= "#define Load${loader}IntoMemory(...) ${loader}_LOAD_SELECT(__VA_ARGS__, Load${loader}IntoMemory4, Load${loader}IntoMemory3)(__VA_ARGS__)\n";
  $s .= "void Print$api(const $base *);\n";
  $s .= "int Parse${api}Query(char *, void *);\n";
  $s .= "int Parse${api}Record(char *, void *);\n";
  $s .= "${api}HashTable *Build${api}HashTable(const $base *, size_t);\n";
  $s .= "${api}Match Find${api}ID(const ${api}HashTable *, const $base *);\n";
  $s .= "void Free${api}Match(${api}Match *);\n";
  $s .= "void Free${api}HashTable(${api}HashTable *);\n";
  $s .= "int Query${api}Memory(const void *, const void *);\n";
  $s .= "int Query${api}Database(MYSQL *, const void *);\n\n";
  $s .= "#endif\n";
  return $s;
}

sub render_source {
  my ($module, $type, $t) = @_;
  my $base = "$module$type";
  my $api = api_name($module, $type);
  my $loader = loader_name($module, $type);
  my @all = @{$t->{columns}}[1 .. $#{$t->{columns}}];
  my @keys = hash_columns($module, $type, $t);
  my $s = "#include \"$base.h\"\n";
  $s .= "#include \"../RecordLoader.h\"\n";
  $s .= "#include <errno.h>\n#include <inttypes.h>\n";
  $s .= "#include <stdio.h>\n#include <stdlib.h>\n";
  $s .= "#include <string.h>\n\n";
  $s .= "struct ${api}HashTable { RecordHashTable *records; };\n\n";
  $s .= "static const RecordField loadFields[] = {\n";
  for my $col (@all) {
    my $m = member_name($t->{table}, $col->{name});
    my $flag = $col->{text} ? 1 : 0;
    $s .= "  {offsetof($base, $m), sizeof((($base *)0)->$m), $flag},\n";
  }
  $s .= "};\n\n";
  $s .= "static const RecordField keyFields[] = {\n";
  for my $col (@keys) {
    my $m = member_name($t->{table}, $col->{name});
    my $flag = $col->{text} ? 1 : 0;
    $s .= "  {offsetof($base, $m), sizeof((($base *)0)->$m), $flag},\n";
  }
  $s .= "};\n\n";

  my $select = 'SELECT ' . join(', ', map { '`' . $_->{name} . '`' } @{$t->{columns}}) . ' FROM `' . $t->{table} . '`';
  $s .= "static int Load${loader}Rows(MYSQL *db, $base **records, size_t *count, size_t limit) {\n";
  $s .= "  if (!records || !count) return EXIT_FAILURE;\n"; 
  $s .= "  *records = NULL;\n";
  $s .= "  *count = 0;\n";
  $s .= "  if (!db) return EXIT_FAILURE;\n";
  $s .= "  const char *select = \"$select\";\n";
  $s .= "  char query[8192];\n";
  $s .= "  int n = limit ? snprintf(query, sizeof query, \"%s LIMIT %zu\", select, limit) : snprintf(query, sizeof query, \"%s\", select);\n";
  $s .= "  if (n < 0 || (size_t)n >= sizeof query) return EXIT_FAILURE;\n";    
  $s .= "  void *data = NULL;\n"; 
  $s .= "  size_t loaded = 0;\n";
  $s .= "  int rc = LoadRecordArray(db, query, sizeof($base), offsetof($base, ID), offsetof($base, nullFields), loadFields, sizeof loadFields / sizeof loadFields[0], &data, &loaded);\n";
  $s .= "  if (rc != EXIT_SUCCESS) {\n"; 
  $s .= "    free(data);\n"; 
  $s .= "    return rc;\n"; 
  $s .= "  }\n"; 
  $s .= "  *records = data;\n"; 
  $s .= "  *count = loaded;\n"; 
  $s .= "  return EXIT_SUCCESS;\n";
  $s .= "}\n\n";
  $s .= "int Load${loader}IntoMemory3(MYSQL *db, $base **records, size_t *count) { return Load${loader}Rows(db, records, count, 0); }\n";
  $s .= "int Load${loader}IntoMemory4(MYSQL *db, $base **records, size_t *count, size_t limit) { return Load${loader}Rows(db, records, count, limit); }\n\n";
  $s .= "void Print$api(const $base *record) {\n"; 
  $s .= "  if (!record) return;\n"; 
  $s .= "  printf(\"ID=%\" PRIu32, record->ID);\n";
  my %ordinal; for my $i (0 .. $#all) { $ordinal{$all[$i]{name}} = $i + 1; }
  for my $col (@all) {
    my $m = member_name($t->{table}, $col->{name}); my $bit = $ordinal{$col->{name}};
    if ($col->{text}) {
      $s .= "  printf(\" $m=%s\", (record->nullFields & (UINT32_C(1) << $bit)) ? \"<NULL>\" : record->$m);\n";
    } else {
      $s .= "  printf(\" $m=%\" PRIu32, record->$m);\n";
    }
  }
  $s .= "  putchar('\\n');\n";
  $s .= "}\n\n";
  
  # CSV parser (empty tokens are NULL); hash key fields are serialized in schema order.
  $s .= "int Parse${api}Query(char *line, void *opaqueRecord) {\n";
  $s .= "  $base *record = opaqueRecord;\n";
  $s .= "  if (!line || !record) return EXIT_FAILURE;\n";
  $s .= "  memset(record, 0, sizeof *record);\n";
  $s .= "  uint32_t nulls = 0;\n";
  $s .= "  char *p = line;\n\n";
    
  for my $i (0 .. $#keys) {
    my $col = $keys[$i]; my $m = member_name($t->{table}, $col->{name}); my $bit = $i + 1;
    if ($i < $#keys) {
      $s .= "  char *token$i = p;\n";
      $s .= "  char *comma$i = strchr(p, ',');\n";
      $s .= "  if (!comma$i) {\n"; 
      $s .= "    fprintf(stderr, \"Too few comma-separated fields for $api\\n\");\n"; 
      $s .= "    return EXIT_FAILURE;\n"; 
      $s .= "  }\n"; 
      $s .= "  *comma$i = '\\0';\n";
      $s .= "  p = comma$i + 1;\n";
    } else {
      $s .= "  char *token$i = p;\n"; 
      $s .= "  if (strchr(p, ',')) {\n"; 
      $s .= "    fprintf(stderr, \"Too many comma-separated fields for $api\\n\");\n"; 
      $s .= "    return EXIT_FAILURE;\n"; 
      $s .= "  }\n"; 
      $s .= "  p += strlen(p);\n";
    }
    $s .= "  if (*token$i == '\\0') {\n"; 
    $s .= "    nulls |= UINT32_C(1) << $bit;\n"; 
    $s .= "  } else {\n";
    if ($col->{text}) {
      $s .= "    size_t len$i = strlen(token$i);\n";  
      $s .= "    if (len$i >= sizeof record->$m) {\n";  
      $s .= "      fprintf(stderr, \"$m exceeds its field capacity\\n\");\n";  
      $s .= "      return EXIT_FAILURE;\n";  
      $s .= "    }\n";  
      $s .= "    memcpy(record->$m, token$i, len$i + 1);\n";
    } else {
      $s .= "    errno = 0;\n"; 
      $s .= "    char *end$i = NULL;\n"; 
      $s .= "    uintmax_t value$i = strtoumax(token$i, &end$i, 10);\n"; 
      $s .= "    if (errno || end$i == token$i || *end$i || value$i > UINT32_MAX) {\n"; 
      $s .= "      fprintf(stderr, \"Invalid unsigned integer for $m: %s\\n\", token$i);\n"; 
      $s .= "      return EXIT_FAILURE;\n"; 
      $s .= "    }\n"; 
      $s .= "    record->$m = (uint32_t)value$i;\n";
    }
    $s .= "  }\n\n";
  }
  $s .= "  if (strchr(p, ',')) {\n"; 
  $s .= "    fprintf(stderr, \"Too many comma-separated fields for $api\\n\");\n"; 
  $s .= "    return EXIT_FAILURE;\n"; 
  $s .= "  }\n"; 
  $s .= "  memcpy(&record->nullFields, &nulls, sizeof nulls);\n"; 
  $s .= "  return EXIT_SUCCESS;\n";
  $s .= "}\n\n";
  
  $s .= "int Parse${api}Record(char *line, void *record) { return Parse${api}Query(line, record); }\n\n";
  $s .= "${api}HashTable *Build${api}HashTable(const $base *records, size_t count) {\n";
  $s .= "  ${api}HashTable *table = calloc(1, sizeof *table);\n";
  $s .= "  if (!table) return NULL;\n";
  $s .= "  table->records = BuildRecordHashTable(records, count, sizeof *records, offsetof($base, ID), offsetof($base, nullFields), keyFields, sizeof keyFields / sizeof keyFields[0]);\n";
  $s .= "  if (!table->records) {\n"; 
  $s .= "    free(table);\n"; 
  $s .= "    return NULL;\n";
  $s .= "  }\n";
  $s .= "  return table;\n";
  $s .= "}\n\n";
  
  $s .= "${api}Match Find${api}ID(const ${api}HashTable *table, const $base *record) {\n";
  $s .= "  ${api}Match m = {.status=RECORD_ERROR};\n";
  $s .= "  if (!table || !record) return m;\n";
  $s .= "  return FindRecordID(table->records, record);\n";
  $s .= "}\n\n";
  
  $s .= "void Free${api}Match(${api}Match *match) { FreeRecordMatch(match); }\n\n";
  
  $s .= "void Free${api}HashTable(${api}HashTable *table) {\n"; 
  $s .= "  if (table) {\n"; 
  $s .= "    FreeRecordHashTable(table->records);\n"; 
  $s .= "    free(table);\n"; 
  $s .= "  }\n"; 
  $s .= "}\n\n";
  
  $s .= "int Query${api}Memory(const void *opaque, const void *record) {\n"; 
  $s .= "  const ${api}HashTable *table = opaque;\n";  
  $s .= "  const $base *query = record;\n";  
  $s .= "  if (!table || !query) return EXIT_FAILURE;\n";  
  $s .= "  ${api}Match m = Find${api}ID(table, query);\n";
  $s .= "  if (m.status == RECORD_FOUND || m.status == RECORD_DUPLICATE) {\n";  
  $s .= "    printf(\"Found %zu matching ID(s):\", m.count);\n";  
  $s .= "    for (size_t i=0; i<m.count; ++i) printf(\" %\" PRIu32, m.IDs[i]);\n";  
  $s .= "    putchar('\\n');\n";  
  $s .= "  } else if (m.status == RECORD_NOT_FOUND) puts(\"Address not found in memory.\");\n";  
  $s .= "  else puts(\"In-memory lookup error.\");\n";  
  $s .= "  int ok = m.status != RECORD_ERROR;\n";  
  $s .= "  Free${api}Match(&m);\n";
  $s .= "  return ok ? EXIT_SUCCESS : EXIT_FAILURE;\n"; 
  $s .= "}\n\n";
  
  # A dynamically grown SQL predicate keeps input values escaped and supports NULL keys.
  $s .= <<'C';
static int AppendSQL(char **buf, size_t *used, size_t *cap, const char *s, size_t n) {
  if (n > SIZE_MAX - *used - 1) return 0;
  size_t need = *used + n + 1;
  if (need > *cap) { 
    size_t c = *cap ? *cap : 256; 
    while (c < need) { 
      if (c > SIZE_MAX/2) {
        c=need; 
        break; 
      } 
      c*=2; 
    } 
    char *p=realloc(*buf,c); 
    if(!p) return 0; 
    *buf=p; 
    *cap=c; 
  }

  memcpy(*buf+*used,s,n); 
  *used+=n; 
  (*buf)[*used]='\0'; 
  return 1;
}
C
  $s .= "\nint Query${api}Database(MYSQL *db, const void *record) {\n";
  $s .= "  const $base *q=record;\n";
  $s .= "  if (!db || !q) return EXIT_FAILURE;\n";
  $s .= "  char *sql=NULL;\n"; 
  $s .= "  size_t used=0,cap=0;\n";
  $s .= "  const char *prefix = \"SELECT `$t->{pk}` FROM `$t->{table}` WHERE \";\n";
  $s .= "  if (!AppendSQL(&sql,&used,&cap,prefix,strlen(prefix))) goto sql_fail;\n\n";
  for my $i (0 .. $#keys) {
    my $col = $keys[$i]; my $m = member_name($t->{table}, $col->{name}); my $bit = $i + 1;
    $s .= "  if ($i && !AppendSQL(&sql,&used,&cap,\" AND \",5)) goto sql_fail;\n";
    $s .= "  if (q->nullFields & (UINT32_C(1) << $bit)) {\n";
    $s .= "    const char *part = \"`$col->{name}` IS NULL\";\n";
    $s .= "    if (!AppendSQL(&sql,&used,&cap,part,strlen(part))) goto sql_fail;\n";
    $s .= "  } else {\n";
    if ($col->{text}) {
      $s .= "    const char *before = \"`$col->{name}` = '\";\n";
      $s .= "    if (!AppendSQL(&sql,&used,&cap,before,strlen(before))) goto sql_fail;\n";
      $s .= "    size_t len=strlen(q->$m);\n";
      $s .= "    if (len > (SIZE_MAX-1)/2) goto sql_fail;\n";
      $s .= "    char *escaped=malloc(len*2+1);\n";
      $s .= "    if (!escaped) goto sql_fail;\n";
      $s .= "    unsigned long escaped_len = mysql_real_escape_string(db,escaped,q->$m,(unsigned long)len);\n";
      $s .= "    int append_ok=AppendSQL(&sql,&used,&cap,escaped,escaped_len) && AppendSQL(&sql,&used,&cap,\"'\",1);\n";
      $s .= "    free(escaped);\n";
      $s .= "    if (!append_ok) goto sql_fail;\n";
    } else {
      $s .= "    char part[96];\n";
      $s .= "    int n=snprintf(part,sizeof part,\"`$col->{name}` = %\" PRIu32,q->$m);\n";
      $s .= "    if(n<0 || (size_t)n>=sizeof part || !AppendSQL(&sql,&used,&cap,part,(size_t)n)) goto sql_fail;\n";
    }
    $s .= "  }\n\n";
  }
  $s .= "  if(mysql_query(db,sql)!=0) {\n"; 
  $s .= "    fprintf(stderr, \"Database lookup failed: %s\\n\",mysql_error(db));\n"; 
  $s .= "    free(sql);\n"; 
  $s .= "    return EXIT_FAILURE;\n"; 
  $s .= "  }\n\n"; 
  $s .= "  free(sql);\n"; 
  $s .= "  MYSQL_RES *res=mysql_store_result(db);\n"; 
  $s .= "  if(!res) {\n"; 
  $s .= "    fprintf(stderr, \"Database result failed: %s\\n\",mysql_error(db));\n"; 
  $s .= "    return EXIT_FAILURE;\n"; 
  $s .= "  }\n\n"; 
  $s .= "  MYSQL_ROW row;\n"; 
  $s .= "  size_t n=0;\n"; 
  $s .= "  while((row=mysql_fetch_row(res))!=NULL) {\n"; 
  $s .= "    if(row[0]) {\n"; 
  $s .= "      printf(\"Database address ID: %s\\n\",row[0]);\n"; 
  $s .= "      n++;\n"; 
  $s .= "    }\n"; 
  $s .= "  }\n\n"; 
  $s .= "  if(mysql_errno(db)) {\n"; 
  $s .= "    fprintf(stderr, \"Database result read failed: %s\\n\",mysql_error(db));\n"; 
  $s .= "    mysql_free_result(res);\n"; 
  $s .= "    return EXIT_FAILURE;\n"; 
  $s .= "  }\n\n"; 
  $s .= "  if(!n) puts(\"Address not found in database.\");\n"; 
  $s .= "  mysql_free_result(res);\n"; 
  $s .= "  return EXIT_SUCCESS;\n\n";
  
  $s .= "sql_fail: free(sql);\n";
  $s .= "  fprintf(stderr, \"Could not build database lookup query.\\n\");\n";
  $s .= "  return EXIT_FAILURE;\n}\n";
  return $s;
}

sub c_string {
  my ($s) = @_;
  $s =~ s/\\/\\\\/g; $s =~ s/"/\\"/g; $s =~ s/\n/\\n/g;
  return $s;
}
  
sub render_check {
  my ($module, $type, $t) = @_;
  my $base = "$module$type"; my $api = api_name($module, $type);
  my $loader = loader_name($module, $type);
  my @keys = hash_columns($module, $type, $t);
  my $table = $t->{table}; my $input_count = scalar @keys;
  my $labels = join(', ', map { member_name($table, $_->{name}) } @keys);
  my $concat = 'SELECT CONCAT(' . join(", ',',", map {
    my $col = $_; my $name = $col->{name};
    my $expr = $col->{text} ? "COALESCE(`$name`, '')" : "COALESCE(CAST(`$name` AS CHAR), '')";
    $expr
  } @keys) . ") AS InputRow FROM `$table` ORDER BY RAND() LIMIT 10";
  my $escaped_query = c_string($concat);
  return <<"C";
#define _POSIX_C_SOURCE 200809L
#include "CommonFiles.h"

static int TestRandom${api}(MYSQL *, const ${api}HashTable *);

int main(int argc, char **argv) {
  PrintWallClockTime("Program started");
  size_t limit = 0;

  if (argc > 1) {
    char *end = NULL; errno = 0; uintmax_t value = strtoumax(argv[1], &end, 10);
    if (errno || end == argv[1] || *end || value > SIZE_MAX) { 
      fprintf(stderr, "Invalid row limit: %s\\n", argv[1]); 
      return EXIT_FAILURE;
    }
    limit = (size_t)value;
  }

  MYSQL *db = DealDB(); 
  if (!db) return EXIT_FAILURE;
  
  $base *rows = NULL; 
  size_t count = 0; 
  ${api}HashTable *table = NULL; 
  int status = EXIT_FAILURE; 
  char line[8192];
  double startMs = NowMilliseconds();
  
  if (Load${loader}IntoMemory(db, &rows, &count, limit) != EXIT_SUCCESS) { 
    fprintf(stderr, "Could not load $table rows.\\n"); 
    goto cleanup; 
  }
  
  table = Build${api}HashTable(rows, count); 
  if (!table) { 
    fprintf(stderr, "Could not build $table hash table.\\n"); 
    goto cleanup; 
  }
  
  PrintWallClockTime("Program loaded"); 
  PrintModuleLoad("$table", count, sizeof(*rows), NowMilliseconds()-startMs);
  TestRandom${api}(db, table);
 
  printf("\\n============================= MATCH $api MEMORY ==========================\\n");
  size_t start = count > 10 ? count - 10 : 0;
  for (size_t i=start; i<count; ++i) { 
    printf("\\nRecord %zu (ID %" PRIu32 "):\\n", i, rows[i].ID); 
    Print$api(&rows[i]); 
  }
  printf("$table module test passed.\\n\\n");
  
  if (count) {
    ${api}Match match = Find${api}ID(table, &rows[0]); 
    printf("Testing loaded row 0 (ID %" PRIu32 "): ", rows[0].ID); 
    if (match.status == RECORD_FOUND || match.status == RECORD_DUPLICATE) printf("hash found %zu matching ID(s).\\n", match.count); 
    else if (match.status == RECORD_NOT_FOUND) puts("NOT FOUND."); 
    else puts("hash lookup error."); 
    Free${api}Match(&match); 
  }
  
  printf("\\nEnter $input_count comma-separated fields in this order:\\n$labels\\nPress Enter on an empty line to quit.\\n");
  while (1) { 
    printf("\\nRecord: "); fflush(stdout); 
    if (!fgets(line,sizeof line,stdin)) { 
      if(ferror(stdin)){
        perror("fgets");
        goto cleanup;
      } 
      break; 
    } 
    line[strcspn(line,"\\r\\n")]='\\0'; 
    if(!line[0]) break; 
    $base query={0}; 
    
    if(Parse${api}Query(line,&query)!=EXIT_SUCCESS) continue; 
    printf("\\n--- In-memory hash lookup ---\\n"); 
    Query${api}Memory(table,&query); 
    printf("\\n--- Database lookup ---\\n"); 
    if(Query${api}Database(db,&query)!=EXIT_SUCCESS) fprintf(stderr,"Database lookup failed.\\n"); 
  }
  status = EXIT_SUCCESS;
  
cleanup:
  PrintWallClockTime("Beginning program cleanup"); 
  Free${api}HashTable(table); 
  free(rows); 
  mysql_close(db); 
  PrintWallClockTime("Program ended"); 
  return status;
}

static int TestRandom${api}(MYSQL *db, const ${api}HashTable *table) {
  const char *query = "$escaped_query";
  if (mysql_query(db,query)!=0) { 
    fprintf(stderr,"Random row query failed: %s\\n",mysql_error(db)); 
    return EXIT_FAILURE; 
  }
  MYSQL_RES *result=mysql_store_result(db); 
  if(!result) { 
    fprintf(stderr,"Could not retrieve random rows: %s\\n",mysql_error(db)); 
    return EXIT_FAILURE; 
  }
  MYSQL_ROW row; size_t testNumber=0; 
  int status=EXIT_SUCCESS;
  while((row=mysql_fetch_row(result))!=NULL) { 
    if(!row[0]) { 
      fprintf(stderr,"Random row query returned NULL.\\n"); 
      status=EXIT_FAILURE; 
      continue; 
    } 
  
    testNumber++; 
    printf("\\n--- Random $api test %zu ---\\n%s\\n",testNumber,row[0]); 
    char input[8192]; 
    int n=snprintf(input,sizeof input,"%s",row[0]); 
    if(n<0||(size_t)n>=sizeof input) {
      fprintf(stderr,"Random row too long.\\n");
      status=EXIT_FAILURE;
      continue;
    } 
    
    $base record={0}; 
    if(Parse${api}Query(input,&record)!=EXIT_SUCCESS) {
      status=EXIT_FAILURE;
      continue;
    } 
    Print$api(&record); 
    printf("In-memory lookup: "); 
    Query${api}Memory(table,&record); 
    printf("Database lookup: "); 
    
    if(Query${api}Database(db,&record)!=EXIT_SUCCESS) status=EXIT_FAILURE; 
  }
  
  if(mysql_errno(db)) {
    fprintf(stderr,"Reading random rows failed: %s\\n",mysql_error(db));
    status=EXIT_FAILURE;
  } 
  mysql_free_result(result); 
  return status;
}
C
}
