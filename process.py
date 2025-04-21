import os
from magenta.models.melody_rnn import melody_rnn_sequence_generator
from magenta.models.shared import sequence_generator_bundle
from magenta.music import midi_io
from magenta.protobuf import generator_pb2, music_pb2
import pretty_midi
from pathlib import Path

def parse_text_file(file_path):
    """Parse the specification file into a dictionary"""
    params = {}
    with open(file_path, 'r') as f:
        for line in f:
            if ':' in line:
                key, value = line.split(':', 1)
                params[key.strip()] = value.strip()
    return params

def create_primer_sequence(params):
    """Create a Magenta NoteSequence from parameters"""
    sequence = music_pb2.NoteSequence()
    
    # Tempo (convert BPM to QPM)
    sequence.tempos.add(qpm=float(params['Specific BPM']))
    
    # Time Signature
    numerator, denominator = params['Time Signature'].split('/')
    sequence.time_signatures.add(
        numerator=int(numerator),
        denominator=int(denominator),
        time=0.0
    )
    
    # Key Signature
    sequence.key_signatures.add(
        key=pretty_midi.key_name_to_key_number(params['Key'].split('/')[0].strip()),
        time=0.0
    )
    
    # Instrument (MIDI program mapping)
    instrument_map = {
        'piano': 0,
        'guitar': 24,
        'drums': 118,
        # Add more instruments as needed
    }
    sequence.instrument_infos.add(
        name=params['Instrument'].lower(),
        program=instrument_map.get(params['Instrument'].lower(), 0)
    )
    
    return sequence

def generate_midi(params, output_dir="magenta_output"):
    # Initialize model
    bundle = sequence_generator_bundle.read_bundle_file(
        'https://storage.googleapis.com/magentadata/models/melody_rnn/basic_rnn.mag'
    )
    generator = melody_rnn_sequence_generator.MelodyRnnSequenceGenerator(
        bundle=bundle,
        steps_per_quarter=4  # Standard for 16th note resolution
    )

    # Create primer sequence
    primer_sequence = create_primer_sequence(params)
    
    # Calculate generation length (measures to steps)
    measures = int(params['Measures'])
    steps_per_measure = 4 * primer_sequence.time_signatures[0].numerator
    total_steps = measures * steps_per_measure

    # Configure generation parameters
    generator_options = generator_pb2.GeneratorOptions()
    generator_options.args['temperature'].float_value = float(
        (int(params['Rhythm Complexity']) + int(params['Melodic Complexity'])) / 20)
    generator_options.generate_sections.add(
        start_time=0,
        end_time=total_steps / generator.steps_per_second
    )

    # Generate sequence
    generated_sequence = generator.generate(primer_sequence, generator_options)

    # Save MIDI
    output_dir = Path(output_dir)
    output_dir.mkdir(exist_ok=True)
    output_path = output_dir / f"{params['Genre']}_{params['Mood']}.mid"
    
    midi_io.sequence_proto_to_midi_file(generated_sequence, str(output_path))
    return output_path

if __name__ == "__main__":
    params = parse_text_file("text.txt")
    result_path = generate_midi(params)
    print(f"Generated MIDI file: {result_path}")