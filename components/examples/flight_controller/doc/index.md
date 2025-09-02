# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Contents
:class: toc
```

## Introduction

The Flight Controller module is responsible for evaluating mission abort conditions and triggering the SelfDestruct system if necessary. It demonstrates a clear MC/DC coverage example.

## Design Considerations

```{spec} Abort on Off-Course
:id: SWDD_FC-100

If the input signal indicates the system is off course, the Flight Controller shall immediately request an abort.
```

```{spec} Abort on Valid Command
:id: SWDD_FC-101

If an abort is commanded and the abort command is valid, the Flight Controller shall request an abort.
```

```{spec} No Abort Otherwise
:id: SWDD_FC-102

If the system is not off course and there is no valid abort command, no abort shall be triggered.
```

```{spec} Update SelfDestruct Interface
:id: SWDD_FC-103

The result of the abort evaluation shall be written to the SelfDestruct interface.
```


## Interfaces

```{spec} Runnable
:id: SWDD_FC-200

The flight controller component is called by its runnable ``flightController()``.
```

```{spec} Off Course Status
:id: SWDD_FC-201

Reads the off course status from the RTE interface ``RteGetOffCourse()``.
```

```{spec} Abort Commanded Signal
:id: SWDD_FC-202

Reads the abort commanded status from the RTE interface ``RteGetAbortCommanded()``.
```

```{spec} Valid Abort Command Signal
:id: SWDD_FC-203

Reads the valid abort command status from the RTE interface ``RteGetValidAbortCommand()``.
```

```{spec} SelfDestruct State
:id: SWDD_FC-204

Writes the abort decision to the SelfDestruct state using the RTE interface ``RteSetSelfDestructState()``.
```

## Internal Behaviour

```{spec} Flow Chart
:id: SWDD_FC-300

The Flight Controller logic can be represented as a simple decision process:
```


```{mermaid}

flowchart TD
    Start([Start])
    abort_commanded{"abort_commanded"}
    valid_abort_command{"valid_abort_command"}
    off_course{"off_course"}
    TRUE([TRUE])
    FALSE([FALSE])

    Start --> abort_commanded
    abort_commanded -- "True" --> valid_abort_command
    valid_abort_command -- "True" --> TRUE
    valid_abort_command -- "False" --> off_course
    abort_commanded -- "False" --> off_course
    off_course -- "True" --> TRUE
    off_course -- "False" --> FALSE

```
