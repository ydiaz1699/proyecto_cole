# Prompt: Generar archivo-mapa.yml

## Objetivo
Analiza el proyecto proporcionado y genera un archivo `archivo-mapa.yml` completo que sirva como mapa técnico exhaustivo del proyecto.

## Qué debe analizar
1. **Estructura de archivos**: Recorrer todos los archivos del proyecto
2. **Código fuente**: Leer src/ para extraer arquitectura, FSM, flujos
3. **Configuración**: Revisar platformio.ini y config.h
4. **Dependencias**: Arduino Core AVR + LiquidCrystal (opcional)
5. **Hardware**: Pines, periféricos, protocolos (STEP/DIR/ADC)

## Reglas de generación
1. Ser exhaustivo pero preciso
2. No inventar: solo documentar lo que existe en el código
3. Priorizar limitaciones críticas (2KB RAM, Timer2 en D3, LCD blocking)
4. Usar valores reales de config.h (500µs, 5000µs, 20ms, etc.)
5. Formato YAML válido
6. Tipo: Embedded/IoT → incluir hardware, pines, consumo

## Criterios de calidad
- Un desarrollador nuevo entiende el proyecto leyendo solo este archivo
- Todas las trampas están en limitaciones_criticas
- Los flujos permiten trazar la ejecución sin abrir el código
