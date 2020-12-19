# Range-Guided Fuzzing

RG-Fuzzer is a range-guided fuzzer. The key idea is to obtain a target range for each targeted branches in the program and drive fuzzing process towards those branches. Such target ranges are obtained from TIS (Trust-In-Software) tool.

The dockerfile related script are under directory of `./dockerdir`

```
$ ./newdoc.sh name-of-container 
$ ./attach.sh name-of-container
```

## Step 1. Obtain target ranges from TIS.

The acquired range information will be stored under directory of `./annotator` in the format of JSON file. e.g. `Recipe_Database.json`

## Step 2. Annotate source code with target ranges automatically.

```
$ cd annotator
$ python3 annotator.py -i /path-to-jsonfile -t /path-to-src/code/of/target
```


## Step 3. Fuzz with range guidance.

```
$ cd daemon_monitor
$ ./run_monitor.sh <targ-program> <AFL/RG/angora> <round-num>
$ $ ./run_fuzzer.sh <targ-program> <AFL/RG/angora> <round-num>
```

## Step 4. Generate coverage report.
```
$ cd daemon_monitor/plot
$ python3 draw.py -i (duration in seconds) -r (rounds of exp, default to 3) -t <program name>
```
It will save the plotting in the form of png file under this directory. 