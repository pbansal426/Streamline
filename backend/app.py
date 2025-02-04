from flask import Flask, jsonify

app = Flask(__name__)

@app.route('/')
def home():
    return jsonify({'message': 'Streamline Backend Running'})

if __name__ == '__main__':
    app.run(port=5000)
