import { Card, CardContent, Typography, Table, TableBody, TableCell, TableHead, TableRow, Chip } from '@mui/material';
import { faultsData } from '../mockData';

export default function FaultPanel() {
  const getSeverityColor = (severity) => {
    switch(severity) {
      case 'CRÍTICO': return 'error';
      case 'ADVERTENCIA': return 'warning';
      default: return 'default';
    }
  };

  return (
    <Card sx={{ bgcolor: '#1a1a1a', border: '1px solid #333' }}>
      <CardContent>
        <Typography variant="h6" color="error" sx={{ mb: 3 }}>REGISTRO DE FALLAS Y ALARMAS ACTIVAS</Typography>
        <Table>
          <TableHead>
            <TableRow>
              <TableCell sx={{ color: '#9e9e9e', fontWeight: 'bold' }}>Timestamp (Evento)</TableCell>
              <TableCell sx={{ color: '#9e9e9e', fontWeight: 'bold' }}>Tipo</TableCell>
              <TableCell sx={{ color: '#9e9e9e', fontWeight: 'bold' }}>Descripción del Error</TableCell>
              <TableCell sx={{ color: '#9e9e9e', fontWeight: 'bold' }}>Severidad</TableCell>
              <TableCell sx={{ color: '#9e9e9e', fontWeight: 'bold' }}>Estado</TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {faultsData.map((fault, index) => (
              <TableRow key={index} hover>
                <TableCell sx={{ color: '#e0e0e0' }}>{fault.timestamp}</TableCell>
                <TableCell sx={{ color: '#e0e0e0' }}>{fault.type}</TableCell>
                <TableCell sx={{ color: '#e0e0e0' }}>{fault.description}</TableCell>
                <TableCell><Chip size="small" label={fault.severity} color={getSeverityColor(fault.severity)} sx={{ borderRadius: 1 }} /></TableCell>
                <TableCell><Chip size="small" label="Solucionada" color="success" sx={{ borderRadius: 1 }} /></TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </CardContent>
    </Card>
  );
}