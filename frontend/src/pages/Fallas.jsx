import { Box } from '@mui/material';
import FaultPanel from '../components/FaultPanel';

export default function Fallas() {
  return (
    <Box sx={{ p: 4, height: 'calc(100vh - 74px)', overflowY: 'auto' }}>
      <FaultPanel />
    </Box>
  );
}