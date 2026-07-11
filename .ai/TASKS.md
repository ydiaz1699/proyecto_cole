# Posicionador Motorizado NEMA17 — Tasks

## En Progreso

- [ ] Implementar código Nivel 2 con FSM y módulos separados
- [ ] Probar en hardware real (motor + driver + Arduino)
- [ ] Ajustar VREF del driver a 1.34V
- [ ] Verificar identificación de bobinas del motor con multímetro

## Pendientes (Backlog)

### Hardware
- [ ] Cortar perfiles 40×40mm según medidas del diseño
- [ ] Soldar estructura base (50×40×30 cm)
- [ ] Montar chumaceras UCP204 y eje Ø20mm
- [ ] Fabricar plato giratorio (Ø25-30cm, acero 5mm)
- [ ] Instalar poleas GT2 (20T motor, 60T plato) y correa
- [ ] Fijar motor NEMA17 con placa de montaje
- [ ] Montar tablero eléctrico (driver, Arduino, fuente)

### Software
- [ ] Calibrar zona muerta del potenciómetro en hardware real
- [ ] Ajustar parámetros de aceleración (ACCEL_INTERVAL_MS) con carga
- [ ] Verificar RPM reales vs calculadas con cronómetro
- [ ] Probar modo LCD (si se incluye pantalla)
- [ ] Probar inversión de dirección bajo carga

### Documentación
- [ ] Fotografiar cada etapa de construcción
- [ ] Grabar video de prueba en vacío y con carga
- [ ] Completar perfil de proyecto para presentación final

## Completados

- [x] Diseño de estructura metálica (medidas definidas)
- [x] Selección de motor (JK42HS40-1684)
- [x] Selección de driver (A4988)
- [x] Código Arduino Nivel 1 (funcional mínimo)
- [x] Cálculos técnicos (torque, reducción, esfuerzos)
- [x] Esquema eléctrico completo
- [x] Cotización de materiales (~596 Bs)
- [x] Documentación .ai/ del proyecto (Nivel 2)
- [x] Análisis de crecimiento de código (Nivel 1 → Nivel 2)
