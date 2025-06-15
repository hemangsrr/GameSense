#Imports

from flask import Flask, request, Response, jsonify
from pydub import AudioSegment
import socket
import pyttsx3
import re
import io


# Flask middleware to handle audio streaming from a socket connection
app = Flask(__name__)

HOST = '127.0.0.1'
PORT = 8080

#Helper functions
def clean_text(text):
    '''
    Cleans the input text by removing unwanted characters and patterns.
    Args:
        text (str): The input text to clean.
        Returns:
        str: The cleaned text.
    '''
    text = re.sub(r"<\|.*?\|>", "", text)
    text = re.sub(r"\bassistant\b", "", text)
    return text.strip()

def split_sentences(text):
    '''
    Splits the input text into sentences based on punctuation.
    Args:
        text (str): The input text to split.
        Returns:
        list: A list of sentences.
    '''
    return re.split(r'(?<=[.!?])\s+', text)

# Function to check if the current fragment is a continuation of the previous one
def is_fragment(prev, curr):
    '''
    Checks if the current fragment is a continuation of the previous one.
    Args:
        prev (str): The previous fragment.
        curr (str): The current fragment.
        Returns:
        bool: True if the current fragment is a continuation, False otherwise.
    '''
    return prev and prev[-1].islower() and curr and curr[0].islower()

# Function to convert text to speech and return audio bytes
def text_to_speech(text):
    '''
    Converts text to speech and returns the audio in WAV format.
    Args:
        text (str): The input text to convert.
        Returns:
        bytes: The audio data in WAV format.
    '''
    engine = pyttsx3.init()
    temp_path = "temp_audio.wav"
    engine.save_to_file(text, temp_path)
    engine.runAndWait()

    # Convert to standard WAV format
    audio = AudioSegment.from_wav(temp_path)
    buffer = io.BytesIO()
    audio.export(buffer, format="wav")
    return buffer.getvalue()

# Flask route to handle audio requests
@app.route('/get_audio', methods=['POST'])
def get_audio():
    '''
    Handles POST requests to /get_audio endpoint.
    Expects JSON input, sends it to a socket server, and streams the audio response.
    Returns:
        Response: A streaming response containing audio data in WAV format.
    '''
    if not request.is_json:
        return jsonify({"error": "Request must be JSON"}), 400

    data = request.get_json()  # Accept any JSON format
    json_str = str(data)  # Convert entire JSON into a string to send to the socket
    print("DEBUG : Recieved JSON - ", json_str)

    def generate():
        with socket.create_connection((HOST, PORT)) as sock:
            sock.sendall(json_str.encode('utf-8'))

            buffer = ""
            while True:
                data = sock.recv(4096)
                if not data:
                    break

                chunk = clean_text(data.decode('utf-8', errors='replace')).strip()
                if not chunk:
                    continue

                if is_fragment(buffer[-1:], chunk[:1]):
                    buffer += chunk
                else:
                    if buffer and not buffer.endswith(" "):
                        buffer += " "
                    buffer += chunk

                sentences = split_sentences(buffer)

                for i, sentence in enumerate(sentences):
                    if i == len(sentences) - 1 and not re.search(r'[.!?]"?$', sentence):
                        buffer = sentence
                    else:
                        audio_bytes = text_to_speech(sentence.strip())
                        yield audio_bytes
                        buffer = ""

    return Response(generate(), mimetype="audio/wav", headers={"Transfer-Encoding": "chunked"})

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)