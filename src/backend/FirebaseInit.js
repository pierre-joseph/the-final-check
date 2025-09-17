// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
import { getFirestore } from "firebase/firestore"; 

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyAyacOq-ZzpqLqHw3wS6mV8ojdt1Yyn3Gg",
  authDomain: "thefinalcheck12.firebaseapp.com",
  projectId: "thefinalcheck12",
  storageBucket: "thefinalcheck12.firebasestorage.app",
  messagingSenderId: "1040693982793",
  appId: "1:1040693982793:web:6ae527bcf535f38dcdf198",
  measurementId: "G-W4LBY0MB0P"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);

// Initialize Firestore
const db = getFirestore(app);
export { db };