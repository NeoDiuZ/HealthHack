import { useState, useEffect, useCallback, useRef } from 'react';

interface WebSocketMessage {
  status: string;
  result?: string;
  message?: string;
  progress?: number;
  confidence?: number;
  label?: string;
}

export const useWebSocket = (baseUrl: string) => {
  const [isConnected, setIsConnected] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const wsRef = useRef<WebSocket | null>(null);

  const connect = useCallback((endpoint: string) => {
    try {
      const ws = new WebSocket(`${baseUrl}${endpoint}`);
      wsRef.current = ws;

      ws.onopen = () => {
        setIsConnected(true);
        setError(null);
      };

      ws.onclose = () => {
        setIsConnected(false);
        wsRef.current = null;
      };

      ws.onerror = () => {
        setError('WebSocket connection failed');
        setIsConnected(false);
        wsRef.current = null;
      };

      return ws;
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to connect');
      setIsConnected(false);
      return null;
    }
  }, [baseUrl]);

  const disconnect = useCallback(() => {
    if (wsRef.current) {
      wsRef.current.close();
      wsRef.current = null;
      setIsConnected(false);
    }
  }, []);

  useEffect(() => {
    return () => {
      disconnect();
    };
  }, [disconnect]);

  return {
    isConnected,
    error,
    connect,
    disconnect,
    currentSocket: wsRef.current
  };
};