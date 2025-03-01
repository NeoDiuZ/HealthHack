const API_BASE_URL = 'http://127.0.0.1:5000/api';

export const startRecording = async (feature: string) => {
  try {
    const response = await fetch(`${API_BASE_URL}/record/start`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
        'Accept': 'application/json',
      },
      credentials: 'include',
      body: JSON.stringify({ feature }),
    });
    return await response.json();
  } catch (error) {
    console.error('Recording error:', error);
    return { status: 'error', message: 'Failed to start recording' };
  }
};

export const stopRecording = async () => {
  try {
    const response = await fetch(`${API_BASE_URL}/record/stop`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
    });
    return await response.json();
  } catch (error) {
    console.error('Stop recording error:', error);
    return { status: 'error', message: 'Failed to stop recording' };
  }
};

export const trainModel = async () => {
  try {
    const response = await fetch(`${API_BASE_URL}/train`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
    });
    return await response.json();
  } catch (error) {
    console.error('Training error:', error);
    return { status: 'error', message: 'Failed to train model' };
  }
};

export const startInference = async () => {
  try {
    const response = await fetch(`${API_BASE_URL}/inference/start`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
    });
    return await response.json();
  } catch (error) {
    console.error('Inference error:', error);
    return { status: 'error', message: 'Failed to start inference' };
  }
};

export const stopInference = async () => {
  try {
    const response = await fetch(`${API_BASE_URL}/inference/stop`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
    });
    return await response.json();
  } catch (error) {
    console.error('Stop inference error:', error);
    return { status: 'error', message: 'Failed to stop inference' };
  }
};

export const getStatus = async () => {
  try {
    const response = await fetch(`${API_BASE_URL}/status`);
    return await response.json();
  } catch (error) {
    console.error('Status error:', error);
    return { status: 'error', message: 'Failed to get status' };
  }
};