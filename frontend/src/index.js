import React from 'react';
import { createRoot } from 'react-dom/client'; // Updated import for React 18
import App from './App.jsx';
import './styles.css';

const container = document.getElementById('root');
const root = createRoot(container); // Create the root
root.render(<App />); // Render the App
