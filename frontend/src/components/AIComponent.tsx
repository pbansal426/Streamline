import React, { useState } from "react";

const AIComponent = () => {
  const [prompt, setPrompt] = useState(""); // User's prompt
  const [response, setResponse] = useState(""); // AI response
  const [loading, setLoading] = useState(false); // Loading state for UI

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setPrompt(e.target.value);
  };

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    setLoading(true); // Start loading
    try {
      const res = await fetch("http://127.0.0.1:8000/ai/prompt", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ prompt }), // Send prompt to backend
      });

      if (!res.ok) {
        throw new Error("Failed to get AI response");
      }

      const data = await res.json();
      setResponse(data.response); // Set AI response
    } catch (error) {
      setResponse("Error: " + error.message); // Handle errors
    } finally {
      setLoading(false); // Stop loading
    }
  };

  return (
    <div>
      <h2>AI Response Generator</h2>
      <form onSubmit={handleSubmit}>
        <input
          type="text"
          value={prompt}
          onChange={handleInputChange}
          placeholder="Enter your prompt here"
        />
        <button type="submit" disabled={loading}>
          {loading ? "Loading..." : "Generate Response"}
        </button>
      </form>

      {response && (
        <div>
          <h3>AI Response:</h3>
          <p>{response}</p>
        </div>
      )}
    </div>
  );
};

export default AIComponent;
