import socket
import magenta
import tensorflow as tf
import os
from magenta.models.music_vae import MusicVAE
from magenta.models.music_vae import TrainedModel
from magenta.models.music_vae import configs
from magenta.music import sequences_lib
from magenta.protobuf import music_pb2

def generate_music(prompt, output_wav_path):
    """Generates music using Magenta's MusicVAE based on a prompt."""
    try:
        # Load the pre-trained MusicVAE model
        config = configs.CONFIG_MAP['cat-mel_256'] # Or another model
        model = TrainedModel(config, batch_size=4, checkpoint_dir_or_path='path/to/music_vae/checkpoints/cat-mel_256.tar') #Replace with your path

        # Convert prompt to a simple melody (you'll need to adapt this)
        note_sequence = music_pb2.NoteSequence()
        note = note_sequence.notes.add()
        note.pitch = 60  # Example pitch (middle C)
        note.start_time = 0.0
        note.end_time = 1.0
        note.velocity = 80

        # Generate music using the model
        generated_sequences = model.sample(n=1, length=256, temperature=1.0) #adjust length and temperature.

        # Convert to audio (using Magenta's utilities)
        sequences_lib.sequence_proto_to_midi_file(generated_sequences[0], "temp.mid")
        os.system(f"fluidsynth -F {output_wav_path} -r 44100 /usr/share/sounds/sf2/FluidR3_GM.sf2 temp.mid") # Replace with your soundfont path.

        return output_wav_path

    except Exception as e:
        print(f"Error generating music: {e}")
        return None

def run_server():
    host = '127.0.0.1'
    port = 12345

    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)

    print(f"Server listening on {host}:{port}")

    while True:
        conn, addr = server_socket.accept()
        print(f"Connection from: {addr}")

        try:
            data = conn.recv(1024).decode('utf-8')
            if data:
                print(f"Received prompt: {data}")
                output_wav_path = "generated_audio.wav"  # Output WAV file
                generated_audio_path = generate_music(data, output_wav_path)

                if generated_audio_path:
                    conn.sendall(generated_audio_path.encode('utf-8'))
                else:
                    conn.sendall(b"ERROR") #send error if the audio generation fails.
            else:
                print("No data received")
        except Exception as e:
            print(f"Error: {e}")
        finally:
            conn.close()

if __name__ == "__main__":
    run_server()