import pickle
import torch
import torch.nn as nn
from transformers import T5Tokenizer
from text2midi.model.transformer_model import Transformer
from huggingface_hub import hf_hub_download
import os
import re

repo_id = "amaai-lab/text2midi"
model_path = hf_hub_download(repo_id=repo_id, filename="pytorch_model.bin")
tokenizer_path = hf_hub_download(repo_id=repo_id, filename="vocab_remi.pkl")

if torch.cuda.is_available():
    device = 'cuda'
elif torch.backends.mps.is_available():
    device = 'mps'
else:
    device = 'cpu'

print(f"Using device: {device}")

with open(tokenizer_path, "rb") as f:
    r_tokenizer = pickle.load(f)

vocab_size = len(r_tokenizer)
print("Vocab size: ", vocab_size)

model = Transformer(vocab_size, 768, 8, 2048, 18, 1024, False, 8, device=device)
model.load_state_dict(torch.load(model_path, map_location=device))
model.eval()
tokenizer = T5Tokenizer.from_pretrained("google/flan-t5-base")

print('Model loaded.')

text_file_path = "text.txt"
try:
    with open(text_file_path, "r") as f:
        lines = f.readlines()

    genre = next((line.split(":", 1)[1].strip() for line in lines if "Genre:" in line), "Unknown Genre")
    instrument = next((line.split(":", 1)[1].strip() for line in lines if "Instrument:" in line), "Unknown Instrument")
    tempo = next((line.split(":", 1)[1].strip() for line in lines if "Specific BPM:" in line), "Unknown Tempo")
    mood = next((line.split(":", 1)[1].strip() for line in lines if "Mood:" in line), "Unknown Mood")

    src = f"Generate a {genre} piece for {instrument} at {tempo} BPM with a {mood} mood."
    print(f'Generating for prompt: "{lines}"')

    if genre != "Unknown Genre" and mood != "Unknown Mood":
        filename = f"{genre.lower()}_{mood.lower()}"
    elif genre != "Unknown Genre":
        filename = genre.lower()
    elif mood != "Unknown Mood":
        filename = mood.lower()
    else:
        filename = "output"

    filename = re.sub(r'[^a-z0-9_]', '', filename)
    filename = filename[:20]

    #  ** Manually set the output directory **
    output_dir = '/Users/prathambansal/Music/Logic/StreamlineMIDI/'

    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    output_midi_path = os.path.join(output_dir, filename + ".mid")

except FileNotFoundError:
    print(f"Error: {text_file_path} not found in the current directory.")
    exit()

inputs = tokenizer(src, return_tensors='pt', padding=True, truncation=True)
input_ids = nn.utils.rnn.pad_sequence(inputs.input_ids, batch_first=True, padding_value=0).to(device)
attention_mask = nn.utils.rnn.pad_sequence(inputs.attention_mask, batch_first=True, padding_value=0).to(device)

output = model.generate(input_ids, attention_mask, max_len=400, temperature=1)
output_list = output[0].tolist()
generated_midi = r_tokenizer.decode(output_list)
generated_midi.dump_midi(output_midi_path)
print(f"Saved to {output_midi_path}")
