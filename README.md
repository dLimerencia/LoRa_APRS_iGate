# LoRa APRS iGate/Digirepeater
## Taller de Integrador
### Grupo 7
Integrantes: 
- Diego Armando Ramirez Mata
- Asdrubal Gerardo Gomez Gutierrez
- Steven Alberto Rojas Rojas


Este firmware está destinado a utilizar placas basadas en ESP32 con módulos LoRa y GPS para vivir en el mundo APRS. 

Especialmente con la board LoRa32 V2.1_1.6

![Imagen de WhatsApp 2024-05-23 a las 21 53 57_eda80156](https://github.com/dLimerencia/LoRa_APRS_iGate_Grupo-7-/assets/100336056/b8e55b53-82b9-466e-8b92-5f86487f4edf)   

![Imagen de WhatsApp 2024-05-23 a las 21 53 57_6e686ad8](https://github.com/dLimerencia/LoRa_APRS_iGate_Grupo-7-/assets/100336056/37c93c65-3e42-42c0-9243-eb4c41651bc5)


Los firmware están diseñado para placas basadas en ESP32 que integran módulos LoRa y GPS, y utilizado para operar en el contexto del mundo APRS (Automatic Packet Reporting System). Siendo APRS un sistema de comunicaciones de radio amateur que utiliza paquetes de datos para transmitir información, entre ellos: posición de estaciones de radio, datos meteorológicos, mensajes cortos, etc. Entonces, este firmware se configura para recopilar datos de ubicación del GPS, transmitirlos a través del módulo LoRa y también para recibir y procesar datos APRS de otras estaciones. Cabe acotar que, es una aplicación interesante que podría ser útil para rastreo y comunicaciones en áreas donde la cobertura de red celular es limitada o inexistente.
Es de vital importancia mencioanr el funcionamiento, ya que, puede ser afectado por varios factores, tanto internos como externos. Dentro de estos factores incluyen: La interferencia electromagnética, los dispositivos electrónicos cercanos que generan interferencia electromagnética en las frecuencias utilizadas por los módulos LoRa y GPS, podría afectar la calidad de la señal y la precisión de los datos. Las condiciones atmosféricas en entornos con mal tiempo o condiciones atmosféricas adversas, como fuertes lluvias o tormentas eléctricas, la señal de GPS y las comunicaciones LoRa podrían verse afectadas, lo que podría disminuir la precisión de la ubicación y la confiabilidad de la comunicación. las obstrucciones físicas como  edificios altos, árboles densos u otras obstrucciones físicas pueden bloquear la señal GPS y afectar la calidad de la conexión LoRa. Los problemas de hardware por ejemplo fallos en los componentes de hardware, como antenas dañadas o conexiones sueltas, pueden interferir con el funcionamiento adecuado del firmware. Tambnién la configuración incorrecta* de los parámetros del firmware, como la frecuencia de transmisión, la tasa de baudios, etc., puede causar problemas en el funcionamiento. A su vez, los problemas de software que provocan errores en el firmware mismo podrían causar mal funcionamiento, como errores en el procesamiento de datos GPS, problemas de comunicación LoRa, etc; así como limitaciones de energía, esto si el sistema no tiene una fuente de energía adecuada o si la batería se agota, podría afectar la capacidad del dispositivo para operar correctamente durante períodos prolongados. Las actualizaciones de firmware mal implementadas podrían introducir errores o incompatibilidades que afecten el funcionamiento del sistema.
