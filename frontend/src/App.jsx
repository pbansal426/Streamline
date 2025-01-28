// src/App.jsx
import React, { useState } from 'react';
import axios from 'axios';
import './styles.css';

export default function App() {
  const [userInput, setUserInput] = useState('');
  const [projectData, setProjectData] = useState(null);
  const [aiResponse, setAIResponse] = useState('');

  const handleAICommand = async () => {
    try {
      // Send the user's prompt to the Flask backend
      const response = await axios.post('http://localhost:5000/process-prompt', {
        text: userInput,
      }, {
        headers: { 'Content-Type': 'application/json' }
      });
      

      // Handle the response from the backend
      const action = response.data.action;
      if (action.type === 'create_project') {
        setProjectData({
          tracks: action.tracks,
          bpm: action.bpm
        });
        setAIResponse(action.message);
      } else if (action.type === 'message') {
        setAIResponse(action.message);
      }

      // Clear the input field
      setUserInput('');
    } catch (error) {
      console.error('Error communicating with the backend:', error);
      setAIResponse('An error occurred. Please try again.');
    }
  };

  return (
    <div className="app-container">
      <h1>AI DAW</h1>
      <div className="chat-interface">
        <input
          value={userInput}
          onChange={(e) => setUserInput(e.target.value)}
          placeholder="Tell the AI what to create..."
          onKeyPress={(e) => {
            if (e.key === 'Enter') handleAICommand();
          }}
        />
        <button onClick={handleAICommand}>Create</button>
      </div>

      {aiResponse && (
        <div className="ai-response">
          <strong>AI:</strong> {aiResponse}
        </div>
      )}

      {projectData && (
        <div className="project-view">
          <h2>Project (BPM: {projectData.bpm})</h2>
          <div className="tracks">
            {projectData.tracks.map((track, index) => (
              <div key={index} className="track">
                🎵 {track}
              </div>
            ))}
          </div>
        </div>
      )}
    </div>
  );
}