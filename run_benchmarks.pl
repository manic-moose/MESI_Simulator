#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Cwd;
use Data::Dumper;

use constant MESI => 0;
use constant MSI  => 1;
use constant MI   => 2;

sub fatal ($) {
    my ($msg) = @_;
    die "-F- $msg\n";
}

sub info ($) {
    my ($msg) = @_;
    print "-I- $msg\n";
}

sub error ($) {
    my ($msg) = @_;
    print "-E- $msg\n";
}

sub warning ($) {
    my ($msg) = @_;
    print "-W- $msg\n";
}

sub runSystemCommand($) {
    my ($command) = @_;
    info("Running System Command: $command");
    my $output = `$command 2>&1`;
    my $retval = $?;
    return ($output,$retval);
}

sub typeToStr ($) {
    my ($type) = @_;
    if ($type == MESI) {
        return "MESI";
    } elsif ($type == MSI) {
        return "MSI";
    } elsif ($type == MI) {
        return "MI";
    } else {
        warning("Invalid controller type: $type");
        return "";
    }
}

sub parseArgs() {
    my $options = {};
    my $options_csv = {};
    
    my $help;
    my $csvonly;
    
    
    GetOptions($options_csv,
        "h" => \$help,
        "csvonly" => \$csvonly,
        "csv=s",
        "numthreads=s",
        "benchmarks=s",
        "burstlength=s",
        "readlatency=s",
        "maxcycles=s",
        "controllers=s",
        "tracedir=s",
        "inputexe=s",
        "logdir=s",
    );
    if (defined($help) && $help) {
        info("Usage:");
        info("./run_benchmarks.pl <options>");
        info(" Arguments:");
        info("  -h                        : Print usage and exit");
        info("  -numthreads  <value_csv>  : CSV of the number of threads to run simulations of. Default is 4. May be a csv of any positive integer");
        info("  -benchmarks  <value_csv>  : CSV list of the names of the benchmarks being run. Should correspond to subdirectories in trace dir. Mandatory.");
        info("  -burstlength <value>      : Burst Length to use for simulations. Default 8");
        info("  -readlatency <value>      : Read latency to use for simluations. Default is 50.");
        info("  -maxcycles   <value>      : Maximum simulation cycles for each simulation. Default is 100000");
        info("  -controllers <value_csv>  : CSV of controller types to simulate. Legal values are mesi,msi, or mi. Default is all controller types.");
        info("  -tracedir    <value>      : Path to root directory that contains benchmark input files. Default is cwd/tracfiles.");
        info("                              It is assumed that there is a subdirectory in tracedir that is named for each benchmark. In that subdirectory, there");
        info("                              must be another subdirectory for each processor count being simulated. Finally, there must be inside each of those directories");
        info("                              a trace file for each thread with the name memtrace.out.# where # corresponds to a thread number.");
        info("  -inputexe    <value>      : Path to simulator executable. Defaults to \$cwd/cache_coherencesim");
        info("  -logdir      <value>      : Log file directory. Defaults to \$cwd/logs");
        info("  -csvonly                  : Skips running the simulations. Parses any logfiles found in the log directory and generates summary csv.");
        info("  -csv         <value>      : Path to csv file output.");
        exit(0);
    }
    
    if (defined($options_csv->{csv})) {
        $options->{csv} = $options_csv->{csv};
    } else {
        $options->{csv} = getcwd."/SUMMARY.csv";
    }
    
    if (defined($csvonly)) {
        $options->{csvonly} = $csvonly;
    } else {
        $options->{csvonly} = 0;
    }
    
    if (defined($options_csv->{logdir})) {
        my $logPath = $options_csv->{logdir};
        $options->{logdir} = $logPath;
        if (!(-d $logPath)) {
            my ($output,$retval) = runSystemCommand("/bin/mkdir -p $logPath");
            if ($retval) {
                error($output);
                fatal("Problem creating log directory: $logPath");
            }
        }
    } else{
        my $logPath = getcwd."/logs";
        $options->{logdir} = $logPath;
        if (!(-d $logPath)) {
            my ($output,$retval) = runSystemCommand("/bin/mkdir -p $logPath");
            if ($retval) {
                error($output);
                fatal("Problem creating log directory: $logPath");
            }
        }
    }
    
    if (defined($options_csv->{inputexe})) {
        my $exePath = $options_csv->{inputexe};
        if (-f $exePath) {
            $options->{inputexe} = $exePath;
        } else {
            fatal("Invalid executable path: $exePath");
        }
    } else{
        my $exePath = getcwd."/cache_coherence_sim";
        if (-f $exePath) {
            $options->{inputexe} = $exePath;
        } else {
            fatal("Invalid executable path: $exePath");
        }
    }
    
    if (defined($options_csv->{numthreads})) {
        my @numThreadsArry = split(/,/, $options_csv->{numthreads});
        foreach my $value (@numThreadsArry) {
            if (!($value =~ m/^\d+$/) || $value < 1) {
                fatal("Number of threads must be a positive integer");
            }
        }
        $options->{numthreads} = \@numThreadsArry;
    } else {
        my @numThreadsArry = (4);
        $options->{numthreads} = \@numThreadsArry;
    }
    if (defined($options_csv->{benchmarks})) {
        my @bmArry = split(/,/, $options_csv->{benchmarks});
        $options->{benchmarks} = \@bmArry;
    } else {
        fatal("Must provide benchmark list csv using -benchmarks");
    }
    if (defined($options_csv->{burstlength})) {
        my $bl = $options_csv->{burstlength};
        if (($bl =~ m/^\d+$/) && ($bl > 0)) {
            $options->{burstlength} = $bl;
        } else {
            fatal("-burstlength must be a positive integer.");
        }
    } else {
        $options->{burstlength} = 8;
    }
    if (defined($options_csv->{readlatency})) {
        my $rl = $options_csv->{readlatency};
        if (($rl =~ m/^\d+$/) && ($rl > 0)) {
            $options->{readlatency} = $rl;
        } else {
            fatal("-readlatency must be a positive integer.");
        }
    } else {
        $options->{readlatency} = 50;
    }
    if (defined($options_csv->{maxcycles})) {
        my $mc = $options_csv->{maxcycles};
        if (($mc =~ m/^\d+$/) && ($mc > 0)) {
            $options->{maxcycles} = $mc;
        } else {
            fatal("-maxcycles must be a positive integer.");
        }
    } else {
        $options->{maxcycles} = 100000;
    }
    if (defined($options_csv->{controllers})) {
        my @cTypes = split(/,/, $options_csv->{controllers});
        my @finalTypes = ();
        if (scalar(@cTypes) > 3) {
            fatal("Only legal controller types are MESI,MSI, and MI (max number of elements in csv argument violated).");
        }
        foreach my $type (@cTypes) {
            if (lc($type) eq "mesi") {
                push(@finalTypes, MESI);
            } elsif (lc($type) eq "msi") {
                push(@finalTypes, MSI);
            } elsif (lc($type) eq "mi") {
                push(@finalTypes, MI);
            } else {
                fatal("Illegal controller type: $type. Must be one of MESI, MSI, or MI");
            }
            $options->{controllers} = \@finalTypes;
        }
    } else {
        $options->{controllers} = [MESI,MSI,MI];
    }
    if (defined($options_csv->{tracedir})) {
        my $tDir = $options_csv->{tracedir};
        if (-d $tDir) {
            $options->{tracedir} = $tDir;
        } else {
            fatal("Cannot read trace dir: $tDir");
        }
    } else {
        my $tDir = getcwd."/tracefiles";
        if (-d $tDir) {
            $options->{tracedir} = $tDir;
        } else {
            fatal("Cannot read trace dir: $tDir");
        }
    }
    
    info("Running benchmarks with following parameters:");
    info("Benchmark List       : ".join(",", @{$options->{benchmarks}}));
    info("Number of Threads    : ".join(",", @{$options->{numthreads}}));
    info("Max Simulation Cycles: $options->{maxcycles}");
    info("Burst Length         : $options->{burstlength}");
    info("Read Latency         : $options->{readlatency}");
    info("Controller Types     : ".join(",", @{$options->{controllers}}));
    info("Trace File Root      : $options->{tracedir}");
    
    verifyInputFilesExist($options);
    return $options;
}

sub verifyInputFilesExist ($) {
    my ($options) = @_;
    
    my $traceDir = $options->{tracedir};
    my $benchmarkList = $options->{benchmarks};
    my $tNumList = $options->{numthreads};
    
    my $foundError = 0;
    
    foreach my $bm (@$benchmarkList) {
        foreach my $pNum (@$tNumList) {
            for (my $i = 0; $i < $pNum; $i++) {
                my $expectedTraceFile = "$traceDir/$bm/$pNum/memtrace.out.${i}";
                if (!(-f $expectedTraceFile)) {
                    $foundError = 1;
                    warning("Cannot find trace file: $expectedTraceFile");
                }
            }
        }
    }
    if ($foundError) {
        fatal("One or more trace files are missing.");
    }
}

sub runBenchmark ($$$$) {
    my ($options,$benchmark,$pCount,$controllerType) = @_;
    
    my $traceDir    = $options->{tracedir};
    my $burstLen    = $options->{burstlength};
    my $readLatency = $options->{readlatency};
    my $simCycles   = $options->{maxcycles};
    my $exePath     = $options->{inputexe};
    my $ctypeStr    = typeToStr($controllerType);
    my $logDir      = $options->{logdir};
    
    my $missingInput = 0;
    my $traceFilePrefix = "$traceDir/$benchmark/$pCount/memtrace.out";
    for (my $i = 0; $i < $pCount; $i++) {
        if (!(-f "${traceFilePrefix}.$i")) {
            warning("Missing ${traceFilePrefix}.$i");
            $missingInput = 1;
        }
    }
    if ($missingInput) {
        error("Missing one or more input trace files.");
        return -1;
    }
    
    my $benchmarkCommand = "$exePath -f $traceFilePrefix -m $simCycles -l $readLatency -b $burstLen -p $pCount -c $controllerType";
    my ($output,$retval) = runSystemCommand($benchmarkCommand);
    my $logFile = "$logDir/${benchmark}.${pCount}.${ctypeStr}.log";
    if ($retval) {
        $logFile = getcwd."/${benchmark}.${pCount}.${ctypeStr}.error.log";
        error("Error running benchmark $benchmark with thread count $pCount and controller type $ctypeStr");
        error("See logfile: $logFile");
    } else {
        info("Completed running benchmark $benchmark with thread count $pCount and controller type $ctypeStr");
        info("Logfile: $logFile");
    }
    writeLogFile($logFile,$output);
}

sub runAllBenchmarks ($) {
    my ($options) = @_;
    my $benchmarksArry = $options->{benchmarks};
    my $pCountArry     = $options->{numthreads};
    my $cTypeArry      = $options->{controllers};
    foreach my $benchmark (@$benchmarksArry) {
        foreach my $pCount (@$pCountArry) { 
            foreach my $cType (@$cTypeArry) {
                my $retVal = runBenchmark($options,$benchmark,$pCount,$cType);
            }
        }
    }
}

sub writeLogFile($$) {
    my ($logFileName, $text) = @_;
    open(my $fh, ">", $logFileName) or fatal("Could not open $logFileName for writing.");
    print $fh $text;
    close($fh);
}

sub getDirectoryList ($) {
    my ($dir) = @_;
    opendir(my $dH, $dir) or fatal("Could not read $dir");
    my @files = readdir($dH);
    closedir($dH);
    return @files;
}

sub generateCSV ($) {
    my ($options) = @_;
    my $logDir      = $options->{logdir};
    
    if (!(-d $logDir)) {
        fatal("Invalid log directory: $logDir");
    }
    
    my @files = getDirectoryList($logDir);
    my $csvData = {};
    
    my $i = 0;
    foreach my $log (sort(@files)) {
        if ($log =~ m/^(\w+)\.(\d+)\.(\w+)\.log$/) {
            my ($bmName,$pCount,$cType) = ($1,$2,$3);
            my $filePath = "$logDir/$log";
            my $logData = parseSimLog($filePath);
            $logData->{benchmark} = $bmName;
            $logData->{processorCount} = $pCount;
            $logData->{controllerType} = $cType;
            $csvData->{"simulation_${i}"} = $logData;
        }
        $i++;
    }
    writeCsvFile($options,$csvData);
    
}

sub writeCsvFile ($$) {
    my ($options,$data) = @_;
    my $csvFileName = $options->{csv};
    info("Writing CSV Summary to $csvFileName");
    open(my $fh, ">", $csvFileName) or fatal("Could not open $csvFileName for writing.");
    my $maxPNumValue = _getMaxProcessors($data);
    my @csvHeader = ("Simulation","Benchmark","Controller","Threads",
                     "TotalMemOps","OverallAvgLatency","MemServicedReads",
                     "TotalHits","TotalMisses","HitRate","BusyBusCycles");
   
    
    for (my $i = 0; $i <= $maxPNumValue; $i++) {
        push(@csvHeader,"P${i}.Operations");
        push(@csvHeader,"P${i}.Latency");
        push(@csvHeader,"P${i}.StoreHits");
        push(@csvHeader,"P${i}.StoreMisses");
        push(@csvHeader,"P${i}.LoadHits");
        push(@csvHeader,"P${i}.LoadMisses");
        push(@csvHeader,"P${i}.HitRate");
    }
    
    
    print $fh join(",", @csvHeader)."\n";
    
    foreach my $simulation (sort(keys(%$data))) {
        my $simData      = $data->{$simulation};
        my @rowData      = ($simulation);
        my $benchmark    = $simData->{benchmark};
        my $controller   = $simData->{controllerType};
        my $threadCount  = $simData->{processorCount};
        my $summaryData  = $simData->{summary};
        my $totalmemops  = $summaryData->{"Total Memory Operations"};
        my $overall_lat  = $summaryData->{"Overall Average Latency"};
        my $mem_serviced = $summaryData->{"Reads Serviced By Memory"};
        my $totalHits    = $summaryData->{"Total Combined Hits"};
        my $totalMisses  = $summaryData->{"Total Combined Misses"};
        my $totalHitRate = $summaryData->{"Combined Hit Rate"};
        my $busyCycles   = $summaryData->{"Bus Busy Cycles"};
        push(@rowData,$benchmark,$controller,$threadCount,$totalmemops,$overall_lat,$mem_serviced,$totalHits,$totalMisses,$totalHitRate,$busyCycles);
        
        my $processorHash = $simData->{processor};
        for (my $i = 0; $i <= $maxPNumValue; $i++) {
            if (exists($processorHash->{$i})) {
                my $pDataHash = $processorHash->{$i};
                my $memOps = $pDataHash->{"Memory Operations"};
                my $avgLat = $pDataHash->{"Average Latency"};
                my $stHits = $pDataHash->{"Store Hits"};
                my $stMiss = $pDataHash->{"Store Misses"};
                my $ldHits = $pDataHash->{"Load Hits"};
                my $ldMiss = $pDataHash->{"Load Misses"};
                my $hitrte = $pDataHash->{"Cache Hit Rate"};
                push(@rowData, $memOps,$avgLat,$stHits,$stMiss,$ldHits,$ldMiss,$hitrte);
            } else {
                push(@rowData,"NA","NA","NA","NA","NA","NA","NA");
            }
        }
        print $fh join(",", @rowData)."\n";
    }
    
    close($fh);
}

sub _getMaxProcessors($) {
    my ($data) = @_;
    
    my $maxPNum = 0;
    foreach my $sim (keys(%$data)) {
        my $simHash = $data->{$sim};
        my $processorHash = $simHash->{processor};
        foreach my $pNum (keys(%$processorHash)) {
            if ($pNum > $maxPNum) {
                $maxPNum = $pNum;
            }
        }
    }
    return $maxPNum;
}

sub parseSimLog ($) {
    my ($logFile) = @_;
    info("Parsing Logfile: $logFile");
    open(my $fh, "<", $logFile) or fatal("Could not open $logFile for reading.");
    my @lines = <$fh>;
    close($fh);
    my $logData = {};
  
    
    my $state = "IDLE_NC";
    my $processorNumber = undef;
    my $simCycles = 0;
    
    
    my $i = 0;
    while ($i < scalar(@lines)) {
        my $line = $lines[$i];
        chomp($line);
        $line =~ s/^\s+//;
        $line =~ s/\s+$//;
        if ($state eq "IDLE_NC") {
            if ($line =~ m/Simulation completed with (\d+) simulation cycles/) {
                $simCycles = $1;
                $state = "IDLE";
            } else {
                $state = "IDLE_NC";
            }
        } elsif ($state eq "IDLE") {
            if ($line =~ m/Processor (\d+):/) {
                $processorNumber = $1;
                $state = "PROCESSOR";
            } elsif ($line =~ m/Summary:/) {
                $state = "SUMMARY";
            } else {
                $state = "IDLE";
            }
        } elsif ($state eq "PROCESSOR") {
            if ($line =~ m/Processor (\d+):/) {
                $processorNumber = $1;
                $state = "PROCESSOR";
            } elsif ($line =~ m/^(.+):(.+)$/) {
                my ($paramName,$value) = ($1,$2);
                $paramName =~ s/^\s+//;
                $paramName =~ s/\s+$//;
                $value =~ s/^\s+//;
                $value =~ s/\s+$//;
                $logData->{processor}->{$processorNumber}->{$paramName} = $value;
            } elsif ($line eq "") {
                $state = "IDLE";
            } elsif ($line =~ m/Summary:/) {
                $state = "SUMMARY";
            }
        } elsif ($state eq "SUMMARY") {
            if ($line =~ m/^(.+):(.+)$/) {
                my ($paramName,$value) = ($1,$2);
                $paramName =~ s/^\s+//;
                $paramName =~ s/\s+$//;
                $value =~ s/^\s+//;
                $value =~ s/\s+$//;
                $logData->{summary}->{$paramName} = $value;
            }
        } else {
            fatal("Invalid state: $state");
        }
        $i++;
    }
    
    return $logData;
}

sub main() {
    my $options = parseArgs();
    if (!$options->{csvonly}) {
        runAllBenchmarks($options);
    }
    generateCSV($options);
}



main();