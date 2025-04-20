#!/usr/bin/env python3
# process.py - Process music generation parameters from StreamlineAI

import json
import sys
import os
from datetime import datetime

def load_json_data(file_path):
    """Load data from a JSON file"""
    try:
        with open(file_path, 'r') as f:
            data = json.load(f)
        print(f"Successfully loaded data from {file_path}")
        return data
    except Exception as e:
        print(f"Error loading JSON file: {e}")
        return None

def process_parameters(data):
    """Process the parameters from the JSON data"""
    processed = {}
    
    # Process tempo
    if 'specificBPM' in data:
        try:
            processed['tempo'] = float(data['specificBPM'])
        except ValueError:
            processed['tempo'] = 120.0  # Default tempo
    elif 'tempoDescription' in data:
        # Map tempo descriptions to actual BPM values
        tempo_map = {
            'Very Slow (40-60 BPM)': 50,
            'Slow (60-80 BPM)': 70,
            'Medium (90-120 BPM)': 100,
            'Fast (130-160 BPM)': 140,
            'Very Fast (170+ BPM)': 180
        }
        processed['tempo'] = tempo_map.get(data['tempoDescription'], 120.0)
    
    # Process time signature
    if 'timeSignature' in data:
        sig = data['timeSignature']
        if '4/4' in sig:
            processed['time_signature'] = (4, 4)
        elif '3/4' in sig:
            processed['time_signature'] = (3, 4)
        elif '6/8' in sig:
            processed['time_signature'] = (6, 8)
        elif '5/4' in sig:
            processed['time_signature'] = (5, 4)
        elif '7/8' in sig:
            processed['time_signature'] = (7, 8)
        elif '12/8' in sig:
            processed['time_signature'] = (12, 8)
        else:
            processed['time_signature'] = (4, 4)  # Default
    
    # Process other parameters
    for key in ['key', 'instrument', 'genre', 'style', 'mood', 'additionalNotes']:
        if key in data:
            processed[key.lower()] = data[key]
    
    # Process numeric values
    for key, default in [('measures', 16), ('rhythmComplexity', 5), ('melodicComplexity', 5)]:
        if key in data:
            try:
                processed[key.lower()] = float(data[key])
            except ValueError:
                processed[key.lower()] = default
    
    return processed

def generate_music(params, output_dir=None):
    """
    Generate music based on the processed parameters.
    This is a placeholder function - you would replace this with your actual music generation code.
    
    Args:
        params (dict): Processed music parameters
        output_dir (str): Directory to save the output
        
    Returns:
        str: Path to the generated music file
    """
    print("\nGenerating music with the following parameters:")
    for key, value in params.items():
        print(f"  {key}: {value}")
    
    # This is where you would call your music generation API or algorithm
    # For example:
    # from music_generation_api import generate
    # result = generate(
    #     tempo=params['tempo'],
    #     key=params['key'],
    #     ...
    # )
    
    # For now, just return a placeholder
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    
    if output_dir:
        os.makedirs(output_dir, exist_ok=True)
        output_path = os.path.join(output_dir, f"generated_music_{timestamp}.mid")
    else:
        # Use the same directory as the input file
        output_path = f"generated_music_{timestamp}.mid"
    
    # Create an empty file to simulate generation
    with open(output_path, 'w') as f:
        f.write(f"# This is a placeholder for generated music\n")
        f.write(f"# Parameters: {json.dumps(params, indent=2)}\n")
    
    print(f"\nMusic would be generated at: {output_path}")
    return output_path

def main():
    # Check if a file path was provided
    if len(sys.argv) < 2:
        print("Usage: python process.py <path_to_json_file>")
        return
    
    # Get the file path from command line arguments
    file_path = sys.argv[1]
    
    # Load the data
    data = load_json_data(file_path)
    if not data:
        return
    
    # Process the parameters
    processed_params = process_parameters(data)
    
    # Create an output directory next to the input file
    input_dir = os.path.dirname(file_path)
    output_dir = os.path.join(input_dir, "GeneratedMusic")
    
    # Generate music
    output_path = generate_music(processed_params, output_dir)
    
    print(f"\nProcessing complete. Output: {output_path}")

if __name__ == "__main__":
    main()