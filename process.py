from transformers import AutoModel, AutoTokenizer, AutoModelForCausalLM

model_name = AutoModel.from_pretrained("skytnt/midi-model")

tokenizer = AutoTokenizer.from_pretrained(model_name)
model = AutoModelForCausalLM.from_pretrained(model_name)

with open("text.txt", "r") as f:
    input_text = f.read()

inputs = tokenizer(input_text, return_tensors="pt")
outputs = model.generate(**inputs, max_length=512, do_sample=True)

generated_text = tokenizer.decode(outputs[0], skip_special_tokens=True)

with open("output.txt", "w") as f:
    f.write(generated_text)
