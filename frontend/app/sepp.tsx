import React from 'react';
import { Circle, Line, ArrowRightIcon } from 'lucide-react';

export default function UseCaseDigram() {
  return (
    <div className="w-full h-[600px] bg-slate-50 p-8 rounded-lg shadow-lg">
      {/* Title */}
      <h2 className="text-2xl font-bold text-center mb-8">AuthINC System - Use Case Diagram</h2>
      
      <div className="relative w-full h-full">
        {/* Actors on left */}
        <div className="absolute left-4 top-1/4 space-y-20">
          <div className="flex flex-col items-center">
            <div className="w-12 h-12 rounded-full border-2 border-blue-500 flex items-center justify-center">
              <Circle className="w-8 h-8 text-blue-500" />
            </div>
            <span className="mt-2 font-medium">Regular User</span>
          </div>
          
          <div className="flex flex-col items-center">
            <div className="w-12 h-12 rounded-full border-2 border-blue-500 flex items-center justify-center">
              <Circle className="w-8 h-8 text-blue-500" />
            </div>
            <span className="mt-2 font-medium">Support Agent</span>
          </div>
          
          <div className="flex flex-col items-center">
            <div className="w-12 h-12 rounded-full border-2 border-blue-500 flex items-center justify-center">
              <Circle className="w-8 h-8 text-blue-500" />
            </div>
            <span className="mt-2 font-medium">Admin</span>
          </div>
        </div>

        {/* Use Cases in center */}
        <div className="absolute left-1/3 top-8 space-y-6">
          <div className="bg-blue-100 rounded-full px-6 py-3 border-2 border-blue-500 w-64 text-center">
            Manage Chat Communications
          </div>
          
          <div className="bg-blue-100 rounded-full px-6 py-3 border-2 border-blue-500 w-64 text-center">
            Handle Notifications
          </div>
          
          <div className="bg-blue-100 rounded-full px-6 py-3 border-2 border-blue-500 w-64 text-center">
            Create Support Tickets
          </div>
          
          <div className="bg-blue-100 rounded-full px-6 py-3 border-2 border-blue-500 w-64 text-center">
            Track Email Status
          </div>
          
          <div className="bg-blue-100 rounded-full px-6 py-3 border-2 border-blue-500 w-64 text-center">
            Manage User Permissions
          </div>
        </div>

        {/* System boundary */}
        <div className="absolute right-8 top-4 bottom-4 w-72 border-2 border-gray-400 rounded-lg">
          <div className="text-center py-2 border-b-2 border-gray-400 font-semibold">
            AuthINC System
          </div>
        </div>
      </div>
    </div>
  );
}