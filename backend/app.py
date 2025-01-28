# flask-backend/app.py
from flask import Flask, request, jsonify
from flask_cors import CORS
from llama_cpp import Llama
import os

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})  # Allow Electron frontend to communicate

# Load a FREE local LLM (No API costs!)
# Download model first: https://huggingface.co/TheBloke/Mistral-7B-Instruct-v0.1-GGUF/resolve/main/mistral-7b-instruct-v0.1.Q4_K_M.gguf
llm = Llama(
    model_path="mistral-7b-instruct-v0.1.Q4_K_M.gguf",  # Place this file in your project folder
    n_ctx=2048  # Adjust based on your RAM
)

@app.route('/process-prompt', methods=['POST'])
def process_prompt():
    user_prompt = request.json.get('text', '')
    
    # Use LOCAL LLM to interpret the prompt
    response = llm.create_chat_completion(
        messages=[{"role": "user", "content": f"""
            You are a music production assistant. Respond in 1-2 sentences.
            User request: {user_prompt}
        """}]
    )
    
    ai_message = response['choices'][0]['message']['content']
    
    # Simple logic for project creation
    if "slow rock song" in user_prompt.lower():
        return jsonify({
            "action": {
                "type": "create_project",
                "tracks": ["drum", "bass", "strings", "acoustic_guitar"],
                "bpm": 70,
                "message": ai_message
            }
        })
    else:
        return jsonify({
            "action": {
                "type": "message",
                "message": ai_message
            }
        })

if __name__ == '__main__':
    app.run(port=5000, debug=True)