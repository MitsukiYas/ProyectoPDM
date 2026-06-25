import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import { ThemeProvider, createTheme, CssBaseline } from '@mui/material';
import Navbar from './components/Navbar';
import Dashboard from './pages/Dashboard';
import Fallas from './pages/Fallas';

const industrialDarkTheme = createTheme({
  palette: {
    mode: 'dark',
    background: { default: '#0f0f0f', paper: '#1a1a1a' },
    primary: { main: '#64b5f6' },
    success: { main: '#2e7d32', contrastText: '#ffffff' },
    error: { main: '#c62828', contrastText: '#ffffff' },
    warning: { main: '#f9a825', contrastText: '#000000' },
    text: { primary: '#ffffff', secondary: '#b0bec5' },
  },
  typography: {
    fontFamily: '"Roboto", "Segoe UI", "Arial", sans-serif',
  },
  components: {
    MuiCard: { styleOverrides: { root: { borderRadius: 4, border: '1px solid #333' } } },
    MuiButton: { styleOverrides: { root: { borderRadius: 4 } } },
  },
});

function App() {
  return (
    <ThemeProvider theme={industrialDarkTheme}>
      <CssBaseline />
      <Router>
          <Navbar />
          <Routes>
            <Route path="/" element={<Dashboard />} />
            <Route path="/fallas" element={<Fallas />} />
          </Routes>
      </Router>
    </ThemeProvider>
  );
}

export default App;