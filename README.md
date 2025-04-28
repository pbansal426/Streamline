# Streamline: Text-to-MIDI VST Plugin

## Overview

Streamline is a VST3 plugin designed to bridge the gap between textual descriptions and musical expression.  It allows users to input text prompts and receive corresponding MIDI output, streamlining the process of generating musical ideas.

## Features

* **Text-to-MIDI Conversion:** Converts textual descriptions into MIDI data.
* **VST3 Plugin:** Integrates seamlessly with most Digital Audio Workstations (DAWs).
* **User-Friendly Workflow:** Simplifies the process of generating music from text.

## How It Works

Streamline leverages the `text2midi` library to generate MIDI data from text.  The process involves the following steps:

1.  **Text Input:** The user provides a textual description of the desired musical output within the VST3 plugin.
2.  **MIDI Generation:** The `text2midi` library processes the text and generates corresponding MIDI information.
3.  **MIDI Output:** The generated MIDI data is output by the VST3 plugin, which can then be used within the user's DAW.

## Installation

1.  **VST Plugin Installation:**
    * Move the VST plugin file (`Streamline/Builds/MacOSX/build/Debug/StreamlineAI.vst3`) to the following directory:
        * `/Library/Audio/Plug-Ins/VST3`
2.  **DAW Setup:**
    * Open your Digital Audio Workstation (DAW) and create a new folder to store MIDI output files.
3.  **VST Plugin Interaction**
     * Open the StreamlineAI VST3 plugin in your DAW. Answer all questions prompted in the plug-in window.
4.  **Terminal Setup:**
    * Open a terminal in the root project directory, and run the following commands (ensure Python 3.7 or higher is installed):
        * `git clone https://github.com/AMAAI-Lab/text2midi`
        * `cd text2midi`
        * `pip3 install -r requirements-mac.txt`
5.  **Output Directory Configuration:**
    * Open the `process.py` file. Locate the line containing `output_dir = ''`, and replace the empty string with the path to the folder you created in your DAW for MIDI output files.
6.  **Running the Processing Script:**
    * In the terminal, execute the following commands:
        * `cd ..`
        * `python3 process.py`
    * Wait for the progress bar to complete, then drag the newly generated MIDI file from the folder in the DAW.

## Dependencies

* Python 3.7+
* `text2midi` library
* A Digital Audio Workstation (DAW) that supports VST3 plugins

## Credits

This project utilizes the `text2midi` library. We would like to acknowledge the authors of the following work:

@inproceedings{bhandari2025text2midi,title={text2midi: Generating Symbolic Music from Captions},author={Keshav Bhandari and Abhinaba Roy and Kyra Wang and Geeta Puri and Simon Colton and Dorien Herremans},booktitle={Proceedings of the 39th AAAI Conference on Artificial Intelligence (AAAI 20)


View the GitHub here: https://github.com/AMAAI-Lab/Text2midi

