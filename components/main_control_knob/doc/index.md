# Software Detailed Design

```{toctree}
:maxdepth: 2
:caption: Table of Contents
```

## Introduction

The main control knob component is responsible for calculating a value between 0 and 100 percent depending on the user's input.

## Design Considerations

```{spec} Arrow Up Key press
:id: SWDD_MCK-100

The Arrow Up key is pressed to increase the percentage value.
```

```{spec} Arrow Down Key press
:id: SWDD_MCK-101

The Arrow Down key is pressed to decrease the percentage value.
```

## Interfaces

```{spec} Runnable
:id: SWDD_MCK-200

The main control knob component is called by its runnable `mainControlKnob()`.
```

```{spec} Arrow Key Input
:id: SWDD_MCK-201

The main control knob component uses the RTE interface `RteIsKeyPressed()` to check if the Arrow Up or Arrow Down key is pressed.
```

```{spec} Main Knob Value Input
:id: SWDD_MCK-202

The main control knob component uses the RTE interface `RteGetMainKnobValue()` to get the current value of the main control knob.
```

```{spec} Main Knob Value Output
:id: SWDD_MCK-203

The main control knob component uses the RTE interface `RteSetMainKnobValue()` to set the new value of the main control knob.
```
