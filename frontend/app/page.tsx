'use client'

import React, { useState, useEffect } from 'react';
import { Card, CardContent, CardHeader, CardTitle, CardDescription } from '@/components/ui/card';
import { Alert, AlertDescription, AlertTitle } from '@/components/ui/alert';
import { Activity, AlertCircle, CheckCircle, XCircle, Loader2, Brain, Waves } from 'lucide-react';
import { Input } from '@/components/ui/input';
import { Button } from '@/components/ui/button';
import { startRecording, stopRecording, startInference, stopInference, getStatus, trainModel } from '../api';

// Custom Button component with updated styling
const MotionButton = ({ 
  children, 
  isRecording, 
  disabled, 
  onClick, 
  className = "",
  isCalibrated = false
}: {
  children: React.ReactNode;
  isRecording?: boolean;
  disabled?: boolean;
  onClick: () => void;
  className?: string;
  isCalibrated?: boolean;
}) => (
  <button
    onClick={onClick}
    disabled={disabled}
    className={`
      w-full
      ${isRecording ? 'bg-destructive text-destructive-foreground hover:bg-destructive/90' : 
        isCalibrated ? 'bg-green-600 text-white hover:bg-green-700' :
        'bg-primary text-primary-foreground hover:bg-primary/90'}
      rounded-md
      transition-colors
      disabled:cursor-not-allowed
      disabled:opacity-50
      p-4
      ${className}
    `}
  >
    {children}
  </button>
);

export default function Home() {
  // State management
  const [features, setFeatures] = useState<string[]>([]);
  const [newFeature, setNewFeature] = useState('');
  const [recordingFeature, setRecordingFeature] = useState<string | null>(null);
  const [recordingStatus, setRecordingStatus] = useState('idle');
  const [timeLeft, setTimeLeft] = useState(15);
  const [prediction, setPrediction] = useState<string | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [isInferring, setIsInferring] = useState(false);
  const [calibratedFeatures, setCalibratedFeatures] = useState<Set<string>>(new Set());
  const [isTraining, setIsTraining] = useState(false);
  const [modelTrained, setModelTrained] = useState(false);

  const isCalibrated = features.length > 0 && calibratedFeatures.size === features.length;

  // Add new feature handler
  const handleAddFeature = () => {
    if (newFeature && !features.includes(newFeature)) {
      setFeatures([...features, newFeature]);
      setNewFeature('');
    }
  };

  // Remove feature handler
  const handleRemoveFeature = (feature: string) => {
    setFeatures(features.filter(f => f !== feature));
    setCalibratedFeatures(prev => {
      const newSet = new Set(prev);
      newSet.delete(feature);
      return newSet;
    });
  };

  // Status polling effect
  useEffect(() => {
    let intervalId: NodeJS.Timeout;
    
    const pollStatus = async () => {
      try {
        const status = await getStatus();
        
        if (status.status === 'error') {
          setError(status.message);
          setRecordingStatus('idle');
          setRecordingFeature(null);
          setTimeLeft(15);
        } else if (status.status === 'prediction') {
          setPrediction(status.prediction);
        }
      } catch (err) {
        console.error('Status polling error:', err);
      }
    };

    if (isInferring) {
      intervalId = setInterval(pollStatus, 100);
    }

    return () => {
      if (intervalId) {
        clearInterval(intervalId);
      }
    };
  }, [isInferring]);

  // Countdown timer effect
  useEffect(() => {
    let timerId: NodeJS.Timeout;
    
    if (recordingStatus === 'recording' && timeLeft > 0) {
      timerId = setInterval(() => {
        setTimeLeft(prev => prev - 1);
      }, 1000);
    }

    return () => {
      if (timerId) {
        clearInterval(timerId);
      }
    };
  }, [recordingStatus, timeLeft]);

  // Timer effect to handle completion
  useEffect(() => {
    if (timeLeft === 0 && recordingStatus === 'recording') {
      handleStopRecording();
      // Immediately mark the motion as calibrated
      if (recordingFeature) {
        setCalibratedFeatures(prev => new Set([...prev, recordingFeature]));
      }
      setRecordingStatus('idle');
      setRecordingFeature(null);
      setTimeLeft(15);
    }
  }, [timeLeft, recordingStatus, recordingFeature]);

  // Recording handlers
  const handleStartRecording = async (feature: string) => {
    try {
      setError(null);
      const response = await startRecording(feature);
      if (response.status === 'success') {
        setRecordingFeature(feature);
        setTimeLeft(15);
        setRecordingStatus('recording');
      } else {
        setError(response.message);
      }
    } catch (err) {
      setError('Failed to start recording');
      console.error(err);
    }
  };

  const handleStopRecording = async () => {
    try {
      if (recordingFeature) {
        await stopRecording();
        console.log('Stopping recording...');
      }
    } catch (err) {
      setError('Failed to stop recording');
      console.error(err);
    }
  };

  // Training handler
  const handleTraining = async () => {
    try {
      setIsTraining(true);
      setError(null);
      const response = await trainModel();
      if (response.status === 'success') {
        setModelTrained(true);
      } else {
        setError(response.message);
      }
    } catch (err) {
      setError('Failed to train model');
      console.error(err);
    } finally {
      setIsTraining(false);
    }
  };

  // Inference handler
  const toggleInference = async () => {
    try {
      setError(null);
      if (isInferring) {
        await stopInference();
        setIsInferring(false);
        setPrediction(null);
      } else {
        const response = await startInference();
        if (response.status === 'success') {
          setIsInferring(true);
        } else {
          setError(response.message);
        }
      }
    } catch (err) {
      setError('Failed to toggle inference');
      console.error(err);
    }
  };

  // Recording Status Component
  const RecordingStatus = () => {
    if (!recordingFeature) return null;
    
    return (
      <Alert className="bg-primary/10 border-primary/20">
        <div className="flex items-center gap-2">
          {recordingStatus === 'completing' ? (
            <>
              <Loader2 className="h-4 w-4 animate-spin text-primary" />
              <AlertTitle className="text-primary">Finalizing Recording</AlertTitle>
            </>
          ) : (
            <>
              <Loader2 className="h-4 w-4 animate-spin text-primary" />
              <AlertTitle className="text-primary">Recording in Progress</AlertTitle>
            </>
          )}
        </div>
        <AlertDescription className="text-primary/80">
          {recordingStatus === 'completing' 
            ? `Saving "${recordingFeature}" control data...`
            : `Think about "${recordingFeature}" for ${timeLeft} seconds.`
          }
        </AlertDescription>
      </Alert>
    );
  };

  return (
    <div className="min-h-screen bg-background p-8">
      <div className="max-w-5xl mx-auto space-y-8">
        {/* Header */}
        <div className="text-center space-y-2">
          <h1 className="text-3xl font-bold tracking-tight flex items-center justify-center gap-2">
            <Brain className="w-8 h-8 text-primary" />
            Neural Feature Calibration
          </h1>
          <p className="text-muted-foreground">One time calibration for new features</p>
        </div>

        {/* Feature Management */}
        <Card>
          <CardHeader>
            <CardTitle className="text-xl flex items-center gap-2">
              <Activity className="w-5 h-5 text-primary" />
              Feature Management
            </CardTitle>
            <CardDescription>
              Add or remove features to calibrate
            </CardDescription>
          </CardHeader>
          <CardContent className="space-y-4">
            <div className="flex gap-2">
              <Input
                placeholder="Enter new feature name"
                value={newFeature}
                onChange={(e) => setNewFeature(e.target.value)}
                onKeyPress={(e) => e.key === 'Enter' && handleAddFeature()}
              />
              <Button onClick={handleAddFeature}>Add Feature</Button>
            </div>
            {features.length === 0 && (
              <Alert>
                <AlertCircle className="h-4 w-4" />
                <AlertTitle>No Features</AlertTitle>
                <AlertDescription>Add at least one feature to begin calibration</AlertDescription>
              </Alert>
            )}
          </CardContent>
        </Card>

        {/* Main Content */}
        <div className="grid gap-8">
          {/* Calibration Card */}
          <Card>
            <CardHeader>
              <CardTitle className="text-xl flex items-center gap-2">
                <Activity className="w-5 h-5 text-primary" />
                System Calibration
              </CardTitle>
              <CardDescription>
                Record sample data for each feature
              </CardDescription>
            </CardHeader>
            <CardContent className="space-y-6">
              <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
                {/* Feature Recording Buttons */}
                {features.map((feature) => (
                  <Card key={feature} className="border-2 border-dashed hover:border-primary/20 transition-colors">
                    <CardContent className="pt-6">
                      <div className="flex justify-between items-center mb-4">
                        <h3 className="font-semibold">{feature}</h3>
                        <Button
                          variant="ghost"
                          size="sm"
                          onClick={() => handleRemoveFeature(feature)}
                          disabled={recordingStatus !== 'idle'}
                        >
                          <XCircle className="w-4 h-4" />
                        </Button>
                      </div>
                      <MotionButton
                        isRecording={recordingFeature === feature}
                        disabled={recordingFeature !== null && recordingFeature !== feature}
                        onClick={() => handleStartRecording(feature)}
                        className="h-32 text-lg"
                        isCalibrated={calibratedFeatures.has(feature)}
                      >
                        <div className="flex flex-col items-center gap-3">
                          {calibratedFeatures.has(feature) ? (
                            <CheckCircle className="w-8 h-8" />
                          ) : (
                            <Activity className={`w-8 h-8 ${recordingFeature === feature ? 'animate-pulse' : ''}`} />
                          )}
                          <div className="space-y-1">
                            <div>Record "{feature}"</div>
                            {recordingFeature === feature && (
                              <div className="text-sm font-normal">{timeLeft}s remaining</div>
                            )}
                          </div>
                        </div>
                      </MotionButton>
                    </CardContent>
                  </Card>
                ))}
              </div>

              {/* Recording Status */}
              <RecordingStatus />
            </CardContent>
          </Card>

          {/* Training Card */}
          {isCalibrated && !modelTrained && (
            <Card>
              <CardHeader>
                <CardTitle className="text-xl flex items-center gap-2">
                  <Brain className="w-5 h-5 text-primary" />
                  Model Training
                </CardTitle>
                <CardDescription>
                  Train the model with recorded data
                </CardDescription>
              </CardHeader>
              <CardContent>
                <MotionButton
                  isRecording={isTraining}
                  onClick={handleTraining}
                  className="h-16 text-lg"
                >
                  {isTraining ? (
                    <span className="flex items-center gap-2 justify-center">
                      <Loader2 className="w-4 h-4 animate-spin" />
                      Training Model...
                    </span>
                  ) : (
                    'Train Model'
                  )}
                </MotionButton>
              </CardContent>
            </Card>
          )}

          {/* Real-time Classification Card */}
          <Card>
            <CardHeader>
              <CardTitle className="text-xl flex items-center gap-2">
                <Waves className="w-5 h-5 text-primary" />
                Real-time Processing
              </CardTitle>
              <CardDescription>
                Start real-time control processing
              </CardDescription>
            </CardHeader>
            <CardContent className="space-y-6">
              <MotionButton
                isRecording={isInferring}
                disabled={!isCalibrated || !modelTrained}
                onClick={toggleInference}
                className="h-16 text-lg"
              >
                {isInferring ? (
                  <span className="flex items-center gap-2 justify-center">
                    <Loader2 className="w-4 h-4 animate-spin" />
                    Stop Processing
                  </span>
                ) : (
                  'Start Real-time Processing'
                )}
              </MotionButton>

              {/* Prediction Display */}
              {isInferring && (
                <Card className="bg-muted border-2">
                  <CardContent className="pt-6">
                    <div className="text-center space-y-2">
                      <p className="text-sm text-muted-foreground">Current Prediction</p>
                      <div className="text-4xl font-bold text-primary">
                        {prediction ? prediction : 'Waiting...'}
                      </div>
                    </div>
                  </CardContent>
                </Card>
              )}
            </CardContent>
          </Card>
        </div>

        {/* Error Display */}
        {error && (
          <Alert variant="destructive">
            <AlertCircle className="h-4 w-4" />
            <AlertTitle>Error</AlertTitle>
            <AlertDescription>{error}</AlertDescription>
          </Alert>
        )}

        {/* Status Footer */}
        <div className="text-center text-sm text-muted-foreground">
          {modelTrained ? (
            <div className="flex items-center justify-center gap-2 text-primary">
              <CheckCircle className="w-4 h-4" />
              System calibrated and model trained
            </div>
          ) : isCalibrated ? (
            <div className="flex items-center justify-center gap-2 text-primary">
              <CheckCircle className="w-4 h-4" />
              Calibration complete - Ready for training
            </div>
          ) : features.length > 0 ? (
            <div className="flex items-center justify-center gap-2">
              <AlertCircle className="w-4 h-4" />
              Please complete calibration for all features
            </div>
          ) : (
            <div className="flex items-center justify-center gap-2">
              <AlertCircle className="w-4 h-4" />
              Please add features to begin
            </div>
          )}
        </div>
      </div>
    </div>
  );
}