<!DOCTYPE TS><TS>
<codec>iso8859-1</codec>
<context>
    <name>AnalogDemDlg</name>
    <message>
        <source>S&amp;tations Dialog...</source>
        <translation type='unfinished'>&amp;Estaciones</translation>
    </message>
    <message>
        <source>E&amp;xit</source>
        <translation type='unfinished'>&amp;Salir</translation>
    </message>
    <message>
        <source>&amp;Sound Card Selection</source>
        <translation type='unfinished'>&amp;Tarjeta de Sonido...</translation>
    </message>
    <message>
        <source>&amp;DRM (digital)</source>
        <translation type='unfinished'>&amp;DRM (digital)</translation>
    </message>
    <message>
        <source>New &amp;AM Acquisition</source>
        <translation type='unfinished'>Nueva Adquisición &amp;AM</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation type='unfinished'>&amp;Ver</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation type='unfinished'>&amp;Opciones</translation>
    </message>
    <message>
        <source>&amp;?</source>
        <translation type='unfinished'>&amp;?</translation>
    </message>
    <message>
        <source>Click on the plot to set the demodulation frequency</source>
        <translation type='unfinished'>Clickear en el gráfico para ajustar la frecuencia de demodulación</translation>
    </message>
    <message>
        <source> Hz</source>
        <translation type='unfinished'>Hz</translation>
    </message>
    <message>
        <source>Carrier&lt;br&gt;Frequency:&lt;b&gt;&lt;br&gt;</source>
        <translation type='unfinished'>Frecuencia de&lt;br&gt;Portadora:&lt;b&gt;&lt;br&gt;</translation>
    </message>
    <message>
        <source>&lt;b&gt;Noise Reduction:&lt;/b&gt; The noise suppression is a frequency domain optimal filter design based algorithm. The noise PSD is estimated utilizing a minimum statistic. A problem of this type of algorithm is that it produces the so called "musical tones". The noise becomes colored and sounds a bit strange. At the same time, the useful signal (which might be speech or music) is also distorted by the algorithm. By selecting the level of noise reduction, a compromise between distortion of the useful signal and actual noise reduction can be made.</source>
        <translation type='unfinished'>&lt;b&gt;Reducción de Ruido:&lt;/b&gt; La supresión de ruido consiste en un algoritmo basado en el diseño de un filtro óptimo en el dominio de la frecuencia. La DEP del ruido se estima utilizando un estadístico mínimo. Un problema de este tipo de algoritmo es que producen los llamados "tonos musicales". El ruido se colorea y suena un tanto extraño. Al mismo tiempo, la señal útil (que puede tratarse de voz o de música) también se distorsiona por el algoritmo. Escogiendo el nivel de reducción de ruido, uno puede ajustar el compromiso existente entre la distorsión de la señal útil y la reducción de ruido real.</translation>
    </message>
    <message>
        <source>&lt;b&gt;AGC (Automatic Gain Control):&lt;/b&gt; Input signals can have a large variation in power due to channel impairments. To compensate for that, an automatic gain control can be applied. The AGC has four settings: Off, Slow, Medium and Fast.</source>
        <translation type='unfinished'>&lt;b&gt;AGC (Automatic Gain Control - Control Automático de Ganancia):&lt;/b&gt; Las señales de entrada presentan una gran variación en potencia debido a las alteraciones del canal. Para compensarlas, se puede aplicar un control automático de la ganancia. Dicho control presenta cuatro posibles configuraciones: Off (Apagado), Slow (Lento), Medium (Medio) y Fast (Alto).</translation>
    </message>
    <message>
        <source>&lt;b&gt;Filter Bandwidth:&lt;/b&gt; A band-pass filter is applied before the actual demodulation process. With this filter, adjacent signals are attenuated. The bandwidth of this filter can be chosen in steps of 1 Hz by using the slider bar. Clicking on the right or left side of the slider leveler will increase/decrease the bandwidth by 1 kHz. &lt;br&gt;The current filter bandwidth is indicated in the spectrum plot by a selection bar.</source>
        <translation type='unfinished'>&lt;b&gt;Ancho de banda del filtro:&lt;/b&gt; Antes del proceso real de demodulación, se aplica un filtro paso banda. Gracias a este filtro, las señales adyacentes se atenúan. El ancho de banda de este filtro se puede escoger en pasos de 1 Hz usando la barra deslizante. Al hacer click a la derecha o a la izquierda del nivelador de deslizamiento, se aumentará/disminuirá el ancho de banda 1 kHz. &lt;br&gt;El ancho de banda actual del filtro se indica en el gráfico del espectro mediante una barra de selección.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Demodulation Type:&lt;/b&gt; The following analog demodulation types are available:&lt;ul&gt;&lt;li&gt;&lt;b&gt;AM:&lt;/b&gt; This analog demodulation type is used in most of the hardware radios. The envelope of the complex base-band signal is used followed by a high-pass filter to remove the DC offset. Additionally, a low pass filter with the same bandwidth as the pass-band filter is applied to reduce the noise caused by non-linear distortions.&lt;/li&gt;&lt;li&gt;&lt;b&gt;LSB / USB:&lt;/b&gt; These are single-side-band (SSB) demodulation types. Only one side of the spectrum is evaluated, the upper side band is used in USB and the lower side band with LSB. It is important for SSB demodulation that the DC frequency of the analog signal is known to get satisfactory results. The DC frequency is automatically estimated by starting a new acquisition or by clicking on the plot.&lt;/li&gt;&lt;li&gt;&lt;b&gt;CW:&lt;/b&gt; This demodulation type can be used to receive CW signals. Only a narrow frequency band in a fixed distance to the mixing frequency is used. By clicking on the spectrum plot, the center position of the band pass filter can be set.&lt;/li&gt;&lt;li&gt;&lt;b&gt;FM:&lt;/b&gt; This is a narrow band frequency demodulation.&lt;/li&gt;&lt;/ul&gt;</source>
        <translation type='unfinished'>&lt;b&gt;Tipo de demodulación:&lt;/b&gt; Están disponibles los siguientes tipos de demodulación analógica:&lt;ul&gt;&lt;li&gt;&lt;b&gt;AM:&lt;/b&gt; Este tipo de demodulación analógica se emplea en la mayoría de las radios hardware. La envolvente de la señal compleja banda base se usa seguida de un filtro paso alto para eliminar el offset de DC. Adicionalmente, se aplica un filtro paso bajo con el mismo ancho de banda que el filtro paso banda para reducir el ruido causado por las distorsiones no lineales.&lt;/li&gt;&lt;li&gt;&lt;b&gt;LSB / USB:&lt;/b&gt; Se tratan de tipos de demodulación de banda unilateral (SSB: Single-side-band). Sólo se tiene en cuenta un lado del espectro; en el caso de USB se evalúa el lado superior y en el caso de LSB se evalúa el inferior. En la demodulación SSB es importante que se conozca la frecuencia DC de la señal analógica para obtener resultados satisfactorios. La frecuencia DC se estima automáticamente al empezar una nueva adquisición o al hacer click sobre el gráfico.&lt;/li&gt;&lt;li&gt;&lt;b&gt;CW:&lt;/b&gt; Este tipo de demodulación se puede emplear para recibir señales CW (Continuous Wave). Sólo se emplea una estrecha banda de frecuencia a una distancia fija de la frecuencia de mezclado. Al hacer click en el gráfico del espectro, se puede ajustar la posición central del filtro paso banda.&lt;/li&gt;&lt;li&gt;&lt;b&gt;FM:&lt;/b&gt; Se trata de una demodulación en frecuencia de banda estrecha.&lt;/li&gt;&lt;/ul&gt;</translation>
    </message>
    <message>
        <source>&lt;b&gt;Mute Audio:&lt;/b&gt; The audio can be muted by checking this box. The reaction of checking or unchecking this box is delayed by approx. 1 second due to the audio buffers.</source>
        <translation type='unfinished'>&lt;b&gt;Silenciar Audio:&lt;/b&gt; Al marcar este cuadro el audio puede silenciarse. Al marcar/desmarcar este cuadro la reacción puede retardarse aproximadamente un segundo, debido a los buffers de audio.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Save Audio as WAV:&lt;/b&gt; Save the audio signal as stereo, 16-bit, 48 kHz sample rate PCM wave file. Checking this box will let the user choose a file name for the recording.</source>
        <translation type='unfinished'>&lt;b&gt;Guardar Audio como WAV:&lt;/b&gt; Guarda la señal de audio como un archivo de onda PCM (Pulse Code Modulation) estéreo de 16 bits y con una tasa de muestreo de 48 kHz. Al marcar este cuadro, el usuario podrá elegir un nombre de archivo para la grabación.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Carrier Frequency:&lt;/b&gt; The (estimated) carrier frequency of the analog signal is shown. (The estimation of this parameter can be done by the Autom Frequency Acquisition which uses the estimated PSD of the input signal and applies a maximum search.)</source>
        <translation type='unfinished'>&lt;b&gt;Frecuencia de portadora:&lt;/b&gt; Muestra la frecuencia de portadora (estimada) de  la señal analógica. La estimación de este parámetro puede hacerse mediante la "Adquisición Automática de Frecuencia", que usa la DEP estimada de la señal de entrada y procede a buscar el máximo.</translation>
    </message>
    <message>
        <source>&lt;b&gt;PLL:&lt;/b&gt; The Phase-Lock-Loop (PLL) tracks the carrier of the modulated received signal. The resulting phase offset between the reference oscillator and the received carrier is displayed in a dial control. If the pointer is almost steady, the PLL is locked. If the pointer of the dial control turns quickly, the PLL is out of lock. To get the PLL locked, the frequency offset to the true carrier frequency must not exceed a few Hz.</source>
        <translation type='unfinished'>&lt;b&gt;PLL:&lt;/b&gt; El PLL (Phase Locked Loop) sigue la portadora de la señal recibida modulada. El "offset" de fase resultante entre el oscilador de referencia y la portadora recibida se muestra en el control del dial. Cuando el puntero está casi parado el PLL está enganchado. En cambio, cuando el puntero del control del dial gira rápidamente significa que el PLL está desenganchado. Para que el PLL se enganche, el "offset" de frecuencia respecto de la verdadera frecuencia de portadora no debe exceder de unos pocos Hertzios.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Auto Frequency Acquisition:&lt;/b&gt; Clicking on the input spectrum plot changes the mixing frequency for demodulation. If the Auto Frequency Acquisition is enabled, the largest peak near the curser is selected.</source>
        <translation type='unfinished'>&lt;b&gt;Adquisición Automática de Frecuencia:&lt;/b&gt; Al hacer click sobre el gráfico del espectro de entrada se cambia la frecuencia de mezcla para la demodulación. Si esta opción está habilitada, se elige el mayor pico cercano al cursor.</translation>
    </message>
    <message>
        <source>&amp;Live Schedule Dialog...</source>
        <translation type='unfinished'>&amp;Programación en directo</translation>
    </message>
</context>
<context>
    <name>AnalogDemDlgBase</name>
    <message>
        <source>System Evaluation</source>
        <translation type='unfinished'>Evaluación del sistema</translation>
    </message>
    <message>
        <source>Indicators</source>
        <translation type='unfinished'>Indicadores</translation>
    </message>
    <message>
        <source>&amp;Waterfall</source>
        <translation type='unfinished'>&amp;Cascada</translation>
    </message>
    <message>
        <source>TextFreqOffset</source>
        <translation type='unfinished'>Offset de frecuencia</translation>
    </message>
    <message>
        <source>PLL
Phase Offset</source>
        <translation type='unfinished'>PLL
Offset de fase</translation>
    </message>
    <message>
        <source>Filter Bandwidth</source>
        <translation type='unfinished'>Ancho de banda del filtro</translation>
    </message>
    <message>
        <source>TextLabelBandWidth</source>
        <translation type='unfinished'>Ancho de banda</translation>
    </message>
    <message>
        <source>Demodulation</source>
        <translation type='unfinished'>Demodulación</translation>
    </message>
    <message>
        <source>AM</source>
        <translation type='unfinished'>AM</translation>
    </message>
    <message>
        <source>LSB</source>
        <translation type='unfinished'>LSB</translation>
    </message>
    <message>
        <source>USB</source>
        <translation type='unfinished'>USB</translation>
    </message>
    <message>
        <source>CW</source>
        <translation type='unfinished'>CW</translation>
    </message>
    <message>
        <source>FM</source>
        <translation type='unfinished'>FM</translation>
    </message>
    <message>
        <source>AGC</source>
        <translation type='unfinished'>AGC</translation>
    </message>
    <message>
        <source>Off</source>
        <translation type='unfinished'>No</translation>
    </message>
    <message>
        <source>Slow</source>
        <translation type='unfinished'>Lento</translation>
    </message>
    <message>
        <source>Med</source>
        <translation type='unfinished'>Medio</translation>
    </message>
    <message>
        <source>Fast</source>
        <translation type='unfinished'>Rápido</translation>
    </message>
    <message>
        <source>Noise Reduction</source>
        <translation type='unfinished'>Reducción de ruido</translation>
    </message>
    <message>
        <source>Low</source>
        <translation type='unfinished'>Baja</translation>
    </message>
    <message>
        <source>High</source>
        <translation type='unfinished'>Alta</translation>
    </message>
    <message>
        <source>Audio</source>
        <translation type='unfinished'>Audio</translation>
    </message>
    <message>
        <source>Mute Audio</source>
        <translation type='unfinished'>Silenciar Audio</translation>
    </message>
    <message>
        <source>Save Audio</source>
        <translation type='unfinished'>Guardar Audio</translation>
    </message>
    <message>
        <source>PLL</source>
        <translation type='unfinished'>PLL</translation>
    </message>
    <message>
        <source>Enable</source>
        <translation type='unfinished'>Habilitar</translation>
    </message>
    <message>
        <source>Auto Frequency Acquisition</source>
        <translation type='unfinished'>Adquisición Automática de Frecuencia</translation>
    </message>
    <message>
        <source>&amp;AMSS</source>
        <translation type='unfinished'>&amp;AMSS</translation>
    </message>
    <message>
        <source>&amp;DRM</source>
        <translation type='unfinished'>&amp;DRM</translation>
    </message>
</context>
<context>
    <name>CAMSSDlg</name>
    <message>
        <source>No AMSS detected</source>
        <translation type='unfinished'>AMSS no detectada</translation>
    </message>
    <message>
        <source>Awaiting AMSS data...</source>
        <translation type='unfinished'>Esperando datos AMSS...</translation>
    </message>
</context>
<context>
    <name>CAMSSDlgBase</name>
    <message>
        <source>AMSS</source>
        <translation type='unfinished'>AMSS</translation>
    </message>
    <message>
        <source>AMSS Decoding</source>
        <translation type='unfinished'>Decodificación AMSS</translation>
    </message>
    <message>
        <source>ServiceLabelAMSS</source>
        <translation type='unfinished'>Etiqueta de servicio AMSS</translation>
    </message>
    <message>
        <source>AMSSInfo</source>
        <translation type='unfinished'>Información AMSS</translation>
    </message>
    <message>
        <source>Block:</source>
        <translation type='unfinished'>Bloque:</translation>
    </message>
    <message>
        <source>Bits:</source>
        <translation type='unfinished'>Bits:</translation>
    </message>
    <message>
        <source>SDC:  </source>
        <translation type='unfinished'>SDC:</translation>
    </message>
    <message>
        <source>................</source>
        <translation type='unfinished'>................</translation>
    </message>
    <message>
        <source>AFS:  </source>
        <translation type='unfinished'>AFS:</translation>
    </message>
    <message>
        <source>AMSS Offset</source>
        <translation type='unfinished'>Offset AMSS</translation>
    </message>
    <message>
        <source>AMSSContryCode</source>
        <translation type='unfinished'>Código de país AMSS</translation>
    </message>
</context>
<context>
    <name>CAboutDlg</name>
    <message>
        <source>is a software implementation of a Digital Radio Mondiale (DRM) receiver. With Dream, DRM broadcasts can be received with a modified analog receiver (SW, MW, LW) and a PC with a sound card.</source>
        <translation type='unfinished'>es una implementación software de un receptor DRM (Digital Radio Mondiale). Con Dream, se pueden recibir difusiones DRM con un receptor analógico (SW, MW, LW) modificado y un PC con una tarjeta de sonido. Esta traducción a castellano ha sido realizada por / Translation to Spanish by: gonzalosuardiaz@gmail.com.</translation>
    </message>
    <message>
        <source>Although this software is going to be distributed as free software under the terms of the GPL this does not mean that its use is free of rights of others. The use may infringe third party IP and thus may not be legal in some countries.</source>
        <translation type='unfinished'>Aunque este software se distribuya como software gratuito bajo los términos de la GPL, no significa que su uso esté exento de derechos de terceros. El uso podría infringir la propiedad intelectual de terceros, lo cual podría no ser legal en algunos países.</translation>
    </message>
    <message>
        <source>This compilation of Dream uses the following libraries:</source>
        <translation type='unfinished'>Esta compilación de Dream usa las siguientes bibliotecas:</translation>
    </message>
    <message>
        <source>Dream, Version </source>
        <translation type='unfinished'>Dream, Versión</translation>
    </message>
    <message>
        <source>Open-Source Software Implementation of a DRM-Receiver</source>
        <translation type='unfinished'>Implementación Software de código fuente abierto de un Receptor DRM</translation>
    </message>
    <message>
        <source>Under the GNU General Public License (GPL)</source>
        <translation type='unfinished'>Bajo la Licencia Pública General (GPL) de GNU</translation>
    </message>
</context>
<context>
    <name>CAboutDlgBase</name>
    <message>
        <source>About Dream</source>
        <translation type='unfinished'>Sobre Dream</translation>
    </message>
    <message>
        <source>TextLabelVersion</source>
        <translation type='unfinished'>Versión</translation>
    </message>
    <message>
        <source>Volker Fischer, Alexander Kurpiers</source>
        <translation type='unfinished'>Volker Fischer, Alexander Kurpiers</translation>
    </message>
    <message>
        <source>Darmstadt University of Technology
Institute for Communication Technology</source>
        <translation type='unfinished'>Darmstadt University of Technology
Institute for Communication Technology</translation>
    </message>
    <message>
        <source>Copyright (C) 2001 - 2005</source>
        <translation type='unfinished'>Copyright (C) 2001 - 2005</translation>
    </message>
    <message>
        <source>TextViewCredits</source>
        <translation type='unfinished'>Créditos</translation>
    </message>
    <message>
        <source>&amp;OK</source>
        <translation type='unfinished'>&amp;OK</translation>
    </message>
</context>
<context>
    <name>CDRMPlot</name>
    <message>
        <source>Channel Impulse Response</source>
        <translation type='unfinished'>Respuesta impulsional del canal</translation>
    </message>
    <message>
        <source>Time [ms]</source>
        <translation type='unfinished'>Tiempo [ms]</translation>
    </message>
    <message>
        <source>IR [dB]</source>
        <translation type='unfinished'>IR [dB]</translation>
    </message>
    <message>
        <source>Guard-interval beginning</source>
        <translation type='unfinished'>Comienzo del Intervalo de Guarda</translation>
    </message>
    <message>
        <source>Guard-interval end</source>
        <translation type='unfinished'>Final del Intervalo de Guarda</translation>
    </message>
    <message>
        <source>Estimated begin of impulse response</source>
        <translation type='unfinished'>Comienzo estimado de la respuesta impulsional</translation>
    </message>
    <message>
        <source>Estimated end of impulse response</source>
        <translation type='unfinished'>Final estimado de la respuesta impulsional</translation>
    </message>
    <message>
        <source>Higher Bound</source>
        <translation type='unfinished'>Limite superior</translation>
    </message>
    <message>
        <source>Lower bound</source>
        <translation type='unfinished'>Límite inferior</translation>
    </message>
    <message>
        <source>Channel Transfer Function / Group Delay</source>
        <translation type='unfinished'>Función de transferencia del canal / Retardo de grupo</translation>
    </message>
    <message>
        <source>Carrier Index</source>
        <translation type='unfinished'>Índice de portadora</translation>
    </message>
    <message>
        <source>TF [dB]</source>
        <translation type='unfinished'>TF [dB]</translation>
    </message>
    <message>
        <source>Group Delay [ms]</source>
        <translation type='unfinished'>Retardo de grupo [ms]</translation>
    </message>
    <message>
        <source>Transf. Fct.</source>
        <translation type='unfinished'>Func. Transferencia</translation>
    </message>
    <message>
        <source>Group Del.</source>
        <translation type='unfinished'>Ret. Grupo</translation>
    </message>
    <message>
        <source>Audio Spectrum</source>
        <translation type='unfinished'>Espectro de Audio</translation>
    </message>
    <message>
        <source>Frequency [kHz]</source>
        <translation type='unfinished'>Frecuencia [kHz]</translation>
    </message>
    <message>
        <source>Rel. Frequency Offset / Sample Rate Offset History</source>
        <translation type='unfinished'>Offset de frec. relativa/ Historial de offset de tasa de muestreo</translation>
    </message>
    <message>
        <source>Time [s]</source>
        <translation type='unfinished'>Tiempo [s]</translation>
    </message>
    <message>
        <source>Sample Rate Offset [Hz]</source>
        <translation type='unfinished'>Offset de tasa de muestreo [Hz]</translation>
    </message>
    <message>
        <source>Freq.</source>
        <translation type='unfinished'>Frec.</translation>
    </message>
    <message>
        <source>Samp.</source>
        <translation type='unfinished'>Muestra</translation>
    </message>
    <message>
        <source>Freq. Offset [Hz] rel. to </source>
        <translation type='unfinished'>Offset de Frec. [Hz] rel. a</translation>
    </message>
    <message>
        <source>Delay / Doppler History</source>
        <translation type='unfinished'>Historial de Retardo / Doppler</translation>
    </message>
    <message>
        <source>Time [min]</source>
        <translation type='unfinished'>Tiempo [min]</translation>
    </message>
    <message>
        <source>Delay [ms]</source>
        <translation type='unfinished'>Retardo [ms]</translation>
    </message>
    <message>
        <source>Doppler [Hz]</source>
        <translation type='unfinished'>Doppler [Hz]</translation>
    </message>
    <message>
        <source>Delay</source>
        <translation type='unfinished'>Retardo</translation>
    </message>
    <message>
        <source>Doppler</source>
        <translation type='unfinished'>Doppler</translation>
    </message>
    <message>
        <source>SNR / Correctly Decoded Audio History</source>
        <translation type='unfinished'>Historial de Audio decodificado correctamente / SNR</translation>
    </message>
    <message>
        <source>SNR [dB]</source>
        <translation type='unfinished'>SNR [dB]</translation>
    </message>
    <message>
        <source>Corr. Dec. Audio / DRM-Frame</source>
        <translation type='unfinished'>Audio Corr. Decodificado / Trama DRM</translation>
    </message>
    <message>
        <source>Shifted Power Spectral Density of Input Signal</source>
        <translation type='unfinished'>DEP desplazada de señal de entrada</translation>
    </message>
    <message>
        <source>PSD [dB]</source>
        <translation type='unfinished'>DEP [dB]</translation>
    </message>
    <message>
        <source>DC carrier</source>
        <translation type='unfinished'>Portadora DC</translation>
    </message>
    <message>
        <source>Shifted PSD</source>
        <translation type='unfinished'>DEP Desplazada</translation>
    </message>
    <message>
        <source>SNR Spectrum (Weighted MER on MSC Cells)</source>
        <translation type='unfinished'>Espectro SNR (MER ponderada en celdas MSC)</translation>
    </message>
    <message>
        <source>WMER [dB]</source>
        <translation type='unfinished'>WMER [dB]</translation>
    </message>
    <message>
        <source>SNR Spectrum</source>
        <translation type='unfinished'>Espectro SNR</translation>
    </message>
    <message>
        <source>Input Spectrum</source>
        <translation type='unfinished'>Espectro de entrada</translation>
    </message>
    <message>
        <source>Input Spectrum [dB]</source>
        <translation type='unfinished'>Espectro de entrada [dB]</translation>
    </message>
    <message>
        <source>Input spectrum</source>
        <translation type='unfinished'>Espectro de entrada</translation>
    </message>
    <message>
        <source>Input PSD</source>
        <translation type='unfinished'>DEP de entrada</translation>
    </message>
    <message>
        <source>Input PSD [dB]</source>
        <translation type='unfinished'>DEP de entrada [dB]</translation>
    </message>
    <message>
        <source>Filter bandwidth</source>
        <translation type='unfinished'>Ancho de banda del filtro</translation>
    </message>
    <message>
        <source>My Grid</source>
        <translation type='unfinished'>Mi cuadrícula</translation>
    </message>
    <message>
        <source>Waterfall Input Spectrum</source>
        <translation type='unfinished'>Espectro de entrada en Cascada</translation>
    </message>
    <message>
        <source>FAC Constellation</source>
        <translation type='unfinished'>Constelación FAC</translation>
    </message>
    <message>
        <source>Real</source>
        <translation type='unfinished'>Real</translation>
    </message>
    <message>
        <source>Imaginary</source>
        <translation type='unfinished'>Imaginaria</translation>
    </message>
    <message>
        <source>SDC Constellation</source>
        <translation type='unfinished'>Constelación SDC</translation>
    </message>
    <message>
        <source>MSC Constellation</source>
        <translation type='unfinished'>Constelación MSC</translation>
    </message>
    <message>
        <source>MSC / SDC / FAC Constellation</source>
        <translation type='unfinished'>Constelación MSC / SDC / FAC</translation>
    </message>
    <message>
        <source>&lt;b&gt;Impulse Response:&lt;/b&gt; This plot shows the estimated Impulse Response (IR) of the channel based on the channel estimation. It is the averaged, Hamming Window weighted Fourier back transformation of the transfer function. The length of PDS estimation and time synchronization tracking is based on this function. The two red dashed vertical lines show the beginning and the end of the guard-interval. The two black dashed vertical lines show the estimated beginning and end of the PDS of the channel (derived from the averaged impulse response estimation). If the "First Peak" timing tracking method is chosen, a bound for peak estimation (horizontal dashed red line) is shown. Only peaks above this bound are used for timing estimation.</source>
        <translation type='unfinished'>&lt;b&gt;Respuesta Impulsional:&lt;/b&gt; Este gráfico muestra la respuesta impulsional (IR) estimada del canal basada en la estimación del canal. Es el promedio de la ventana de Hamming de la Transformada de Fourier inversa ponderada de la función de transferencia. La longitud de la estimación de la PDS y el seguimiento de la sincronización temporal están basados en esta función. Las dos líneas rojas verticales discontinuas muestran el comienzo y el final del intervalo de guarda. Las dos líneas negras verticales discontinuas muestran el comienzo y el final estimados de la PDS del canal (derivado de la estimación de la respuesta impulsional media). Si se selecciona el método de seguimiento temporal "Primer pico", se muestra un límite para la estimación del pico (línea roja horizontal discontinua). Únicamente se usan los picos por encima de este límite para la estimación de tiempos.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Transfer Function / Group Delay:&lt;/b&gt; This plot shows the squared magnitude and the group delay of the estimated channel at each sub-carrier.</source>
        <translation type='unfinished'>&lt;b&gt;Función de Transferencia / Retardo de Grupo:&lt;/b&gt; Este gráfico muestra la el módulo al cuadrado y el retardo al cuadrado del canal estimado en cada subportadora.</translation>
    </message>
    <message>
        <source>&lt;b&gt;FAC, SDC, MSC:&lt;/b&gt; The plots show the constellations of the FAC, SDC and MSC logical channel of the DRM stream. Depending on the current transmitter settings, the SDC and MSC can have 4-QAM, 16-QAM or 64-QAM modulation.</source>
        <translation type='unfinished'>&lt;b&gt;FAC, SDC, MSC:&lt;/b&gt; Los gráficos muestran las constelaciones de los canales lógicos FAC, SDC y MSC del flujo DRM. Dependiendo de la configuración del transmisor actual, el SDC y el MSC pueden tener modulaciones 4-QAM, 16-QAM y 64-QAM.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Shifted PSD:&lt;/b&gt; This plot shows the estimated Power Spectrum Density (PSD) of the input signal. The DC frequency (red dashed vertical line) is fixed at 6 kHz. If the frequency offset acquisition was successful, the rectangular DRM spectrum should show up with a center frequency of 6 kHz. This plot represents the frequency synchronized OFDM spectrum. If the frequency synchronization was successful, the useful signal really shows up only inside the actual DRM bandwidth since the side loops have in this case only energy between the samples in the frequency domain. On the sample positions outside the actual DRM spectrum, the DRM signal has zero crossings because of the orthogonality. Therefore this spectrum represents NOT the actual spectrum but the "idealized" OFDM spectrum.</source>
        <translation type='unfinished'>&lt;b&gt;DEP Desplazada:&lt;/b&gt; Este gráfico muestra la Densidad Espectral de Potencia (DEP) de la señal de entrada. La frecuencia DC (línea roja vertical discontinua) se fija a 6 kHz. Si la adquisición del offset de frecuencia fue satisfactoria, el espectro rectangular DRM debería aparecer con una frecuencia central de 6 kHz. Este gráfico representa el espectro OFDM sincronizado en frecuencia. Si la sincronización en frecuencia fue satisfactoria, la señal útil se muestra realmente sólo dentro del ancho de banda real DRM dado que los bucles laterales en este caso sólo tienen energía entre las muestras en el dominio de la frecuencia. En las posiciones de muestras fuera del espectro DRM real, la señal DRM presenta cero cruces debido a la ortogonalidad. Por lo tanto, este espectro NO representa el espectro real sino el espectro "OFDM" idealizado.</translation>
    </message>
    <message>
        <source>&lt;b&gt;SNR Spectrum (Weighted MER on MSC Cells):&lt;/b&gt; This plot shows the Weighted MER on MSC cells for each carrier separately.</source>
        <translation type='unfinished'>&lt;b&gt;Espectro SNR (MER ponderada en Celdas MSC):&lt;/b&gt; Este gráfico muestra la MER ponderada en celdas MSC para cada portadora de forma separada.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Input Spectrum:&lt;/b&gt; This plot shows the Fast Fourier Transformation (FFT) of the input signal. This plot is active in both modes, analog and digital. There is no averaging applied. The screen shot of the Evaluation Dialog shows the significant shape of a DRM signal (almost rectangular). The dashed vertical line shows the estimated DC frequency. This line is very important for the analog AM demodulation. Each time a new carrier frequency is acquired, the red line shows the selected AM spectrum. If more than one AM spectrums are within the sound card frequency range, the strongest signal is chosen.</source>
        <translation type='unfinished'>&lt;b&gt;Espectro de entrada:&lt;/b&gt; Este gráfico muestra la FFT (Transformada Rápida de Fourier) de la señal de entrada. Esta representación está activa en ambos modos, tanto digital como analógico. No se aplica promediado. La captura de pantalla de la ventana de Evaluación muestra la forma significativa de una señal DRM (casi rectangular). La línea vertical discontinua muestra la frecuencia DC estimada. Esta línea es muy importante para la demodulación analógica de AM. Cada vez que se adquiere una nueva frecuencia de portadora, la línea roja muestra el espectro AM seleccionado. Si hay más de un espectro AM dentro del rango de frecuencia de la tarjeta de sonido, se escoge la señal más fuerte.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Input PSD:&lt;/b&gt; This plot shows the estimated power spectral density (PSD) of the input signal. The PSD is estimated by averaging some Hamming Window weighted Fourier transformed blocks of the input signal samples. The dashed vertical line shows the estimated DC frequency.</source>
        <translation type='unfinished'>&lt;b&gt;DEP de Entrada:&lt;/b&gt; Este gráfico muestra la densidad espectral de potencia (DEP) de la señal de entrada. La DEP se estima promediando algunos bloques de las muestras de la señal de entrada a los que se le ha aplicado la ventana de Hamming de la Transformada de Fourier. La línea vertical discontinua muestra la frecuencia DC estimada.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Audio Spectrum:&lt;/b&gt; This plot shows the averaged audio spectrum of the currently played audio. With this plot the actual audio bandwidth can easily determined. Since a linear scale is used for the frequency axis, most of the energy of the signal is usually concentrated on the far left side of the spectrum.</source>
        <translation type='unfinished'>&lt;b&gt;Espectro de Audio:&lt;/b&gt; Este gráfico muestra el espectro de audio promediado del audio que se reproduce actualmente. Mediante esta representación, se puede determinar fácilmente el ancho de banda de audio real. Dado que para el eje de frecuencia se emplea una escala lineal, la mayor parte de la energía de la señal se concentra normalmente en el extremo izquierdo del espectro.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Frequency Offset / Sample Rate Offset History:&lt;/b&gt; The history of the values for frequency offset and sample rate offset estimation is shown. If the frequency offset drift is very small, this is an indication that the analog front end is of high quality.</source>
        <translation type='unfinished'>&lt;b&gt;Espectro de Audio:&lt;/b&gt; Este gráfico muestra el espectro de audio promediado del audio que se reproduce actualmente. Mediante esta representación, se puede determinar fácilmente el ancho de banda de audio real. Dado que para el eje de frecuencia se emplea una escala lineal, la mayor parte de la energía de la señal se concentra normalmente en el extremo izquierdo del espectro.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Doppler / Delay History:&lt;/b&gt; The history of the values for the Doppler and Impulse response length is shown. Large Doppler values might be responsable for audio drop-outs.</source>
        <translation type='unfinished'>&lt;b&gt;Historial de Retardo / Doppler:&lt;/b&gt; Muestra el historial de valores para la longitud Doppler y de la respuesta impulsional. Valores de Doppler muy grandes podrían ser los causantes de caídas del audio.</translation>
    </message>
    <message>
        <source>&lt;b&gt;SNR History:&lt;/b&gt; The history of the values for the SNR and correctly decoded audio blocks is shown. The maximum achievable number of correctly decoded audio blocks per DRM frame is 10 or 5 depending on the audio sample rate (24 kHz or 12 kHz AAC core bandwidth).</source>
        <translation type='unfinished'>&lt;b&gt;Historial de SNR:&lt;/b&gt; Muestra el historial de valores para la relación señal a ruido (SNR) y bloques de audio correctamente decodificados. El número máximo que se puede conseguir de bloques de audio correctamente decodificados es de 10 ó de 5 dependiendo de si la tasa de muestreo del audio es de 24 kHz o de 12 kHz (ancho de banda esencial AAC).</translation>
    </message>
    <message>
        <source>&lt;b&gt;Waterfall Display of Input Spectrum:&lt;/b&gt; The input spectrum is displayed as a waterfall type. The different colors represent different levels.</source>
        <translation type='unfinished'>&lt;b&gt;Representación en Cascada del espectro de entrada:&lt;/b&gt; El espectro de entrada se muestra en modo cascada. Los diferentes colores representan diferentes niveles.</translation>
    </message>
</context>
<context>
    <name>CDreamHelpMenu</name>
    <message>
        <source>What&apos;s &amp;This</source>
        <translation type='unfinished'>Qué es es&amp;to</translation>
    </message>
    <message>
        <source>&amp;About...</source>
        <translation type='unfinished'>&amp;Sobre...</translation>
    </message>
</context>
<context>
    <name>CEPGDlgbase</name>
    <message>
        <source>Programme Guide</source>
        <translation type='unfinished'>Guía de Programa</translation>
    </message>
    <message>
        <source>Guide</source>
        <translation type='unfinished'>Guía</translation>
    </message>
    <message>
        <source>Name</source>
        <translation type='unfinished'>Nombre</translation>
    </message>
    <message>
        <source>Description</source>
        <translation type='unfinished'>Descripción</translation>
    </message>
    <message>
        <source>Genre</source>
        <translation type='unfinished'>Género</translation>
    </message>
    <message>
        <source>New Item</source>
        <translation type='unfinished'>Nuevo objeto</translation>
    </message>
    <message>
        <source>Basic Profile Data</source>
        <translation type='unfinished'>Datos de perfil básico</translation>
    </message>
    <message>
        <source>Advanced Profile Data</source>
        <translation type='unfinished'>Datos de perfil avanzado</translation>
    </message>
    <message>
        <source>EPG decoding is disabled</source>
        <translation type='unfinished'>Decodifiación EPG deshabilitada</translation>
    </message>
    <message>
        <source>Time [UTC]</source>
        <translation type='unfinished'>Hora [UTC]</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
</context>
<context>
    <name>CGeneralSettingsDlgBase</name>
    <message>
        <source>General settings</source>
        <translation type='unfinished'>Opciones Generales</translation>
    </message>
    <message>
        <source>&amp;OK</source>
        <translation type='unfinished'>&amp;OK</translation>
    </message>
    <message>
        <source>Receiver coordinates</source>
        <translation type='unfinished'>Coordenadas del receptor</translation>
    </message>
    <message>
        <source>Latitude   </source>
        <translation type='unfinished'>Latitud</translation>
    </message>
    <message>
        <source>&apos;</source>
        <translation type='unfinished'>&apos;</translation>
    </message>
    <message>
        <source>N/S</source>
        <translation type='unfinished'>N/S</translation>
    </message>
    <message>
        <source>Longitude</source>
        <translation type='unfinished'>Longitud</translation>
    </message>
    <message>
        <source>E/W</source>
        <translation type='unfinished'>E/W</translation>
    </message>
</context>
<context>
    <name>CLiveScheduleDlgBase</name>
    <message>
        <source>Live Schedule</source>
        <translation type='unfinished'>Programación en Directo</translation>
    </message>
    <message>
        <source>Column 1</source>
        <translation type='unfinished'>Columna 1</translation>
    </message>
    <message>
        <source>New Item</source>
        <translation type='unfinished'>Nuevo objeto</translation>
    </message>
    <message>
        <source>TextLabelUTCTime</source>
        <translation type='unfinished'>Hora UTC</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
    <message>
        <source>F&amp;reeze</source>
        <translation type='unfinished'>Pa&amp;rar</translation>
    </message>
</context>
<context>
    <name>CMultSettingsDlgBase</name>
    <message>
        <source>Multimedia settings</source>
        <translation type='unfinished'>Opciones Multimedia</translation>
    </message>
    <message>
        <source>MOT Broadcast Web Site</source>
        <translation type='unfinished'>Página Web de Difusión MOT</translation>
    </message>
    <message>
        <source>Add refresh header of seconds</source>
        <translation type='unfinished'>Añadir cabecera de segundos de refresco</translation>
    </message>
    <message>
        <source>Clear cache</source>
        <translation type='unfinished'>Borrar caché</translation>
    </message>
    <message>
        <source>EPG - Electronic Programme Guide</source>
        <translation type='unfinished'>EPG - Guía Electrónica de Programa</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
</context>
<context>
    <name>CSoundCardSelMenu</name>
    <message>
        <source>Sound &amp;In</source>
        <translation type='unfinished'>&amp;Entrada de Sonido</translation>
    </message>
    <message>
        <source>Sound &amp;Out</source>
        <translation type='unfinished'>&amp;Salida de Sonido</translation>
    </message>
</context>
<context>
    <name>CStationsDlgBase</name>
    <message>
        <source>Stations</source>
        <translation type='unfinished'>Estaciones</translation>
    </message>
    <message>
        <source>Column 1</source>
        <translation type='unfinished'>Columna 1</translation>
    </message>
    <message>
        <source>New Item</source>
        <translation type='unfinished'>Nuevo objeto</translation>
    </message>
    <message>
        <source>TextLabelUTCTime</source>
        <translation type='unfinished'>Hora UTC</translation>
    </message>
    <message>
        <source>kHz</source>
        <translation type='unfinished'>kHz</translation>
    </message>
    <message>
        <source>S-Meter [dB]
(relative to S9):</source>
        <translation type='unfinished'>Parámetros S [dB]
(relativos a S9):</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
</context>
<context>
    <name>EPGDlg</name>
    <message>
        <source>no basic profile data</source>
        <translation type='unfinished'>Sin datos de perfil básico</translation>
    </message>
    <message>
        <source>no advanced profile data</source>
        <translation type='unfinished'>Sin datos de perfil avanzado</translation>
    </message>
    <message>
        <source>no data</source>
        <translation type='unfinished'>Sin datos</translation>
    </message>
</context>
<context>
    <name>FDRMDialog</name>
    <message>
        <source>&amp;Evaluation Dialog...</source>
        <translation type='unfinished'>Ventana de &amp;Evaluación...</translation>
    </message>
    <message>
        <source>M&amp;ultimedia Dialog...</source>
        <translation type='unfinished'>Ventana M&amp;ultimedia...</translation>
    </message>
    <message>
        <source>S&amp;tations Dialog...</source>
        <translation type='unfinished'>Ventana de Es&amp;taciones...</translation>
    </message>
    <message>
        <source>&amp;Live Schedule Dialog...</source>
        <translation type='unfinished'>Programación en &amp;Directo...</translation>
    </message>
    <message>
        <source>&amp;Programme Guide...</source>
        <translation type='unfinished'>Guía de &amp;Programas...</translation>
    </message>
    <message>
        <source>E&amp;xit</source>
        <translation type='unfinished'>&amp;Salir</translation>
    </message>
    <message>
        <source>&amp;Sound Card Selection</source>
        <translation type='unfinished'>Elegir Tarjeta de &amp;Sonido</translation>
    </message>
    <message>
        <source>&amp;AM (analog)</source>
        <translation type='unfinished'>&amp;AM (analógico)</translation>
    </message>
    <message>
        <source>New &amp;DRM Acquisition</source>
        <translation type='unfinished'>Nueva Adquisición &amp;DRM</translation>
    </message>
    <message>
        <source>Set D&amp;isplay Color...</source>
        <translation type='unfinished'>Ajustar Color de V&amp;isualización...</translation>
    </message>
    <message>
        <source>&amp;Blue / White</source>
        <translation type='unfinished'>&amp;Azul / Blanco</translation>
    </message>
    <message>
        <source>&amp;Green / Black</source>
        <translation type='unfinished'>&amp;Verde / Blanco</translation>
    </message>
    <message>
        <source>B&amp;lack / Grey</source>
        <translation type='unfinished'>&amp;Negro / Gris</translation>
    </message>
    <message>
        <source>&amp;Plot Style</source>
        <translation type='unfinished'>Ti&amp;po de Gráfico</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation type='unfinished'>&amp;Ver</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation type='unfinished'>&amp;Opciones</translation>
    </message>
    <message>
        <source>&amp;?</source>
        <translation type='unfinished'>&amp;?</translation>
    </message>
    <message>
        <source>Stations</source>
        <translation type='unfinished'>Estaciones</translation>
    </message>
    <message>
        <source>Live Schedule</source>
        <translation type='unfinished'>Programación en Directo</translation>
    </message>
    <message>
        <source>Programme Guide</source>
        <translation type='unfinished'>Guía de Programas</translation>
    </message>
    <message>
        <source>System Evaluation</source>
        <translation type='unfinished'>Evaluación del Sistema</translation>
    </message>
    <message>
        <source>Multimedia</source>
        <translation type='unfinished'>Multimedia</translation>
    </message>
    <message>
        <source> kbps</source>
        <translation type='unfinished'>kbps</translation>
    </message>
    <message>
        <source>No Service</source>
        <translation type='unfinished'>Sin Servicio</translation>
    </message>
    <message>
        <source> + EPG</source>
        <translation type='unfinished'>+ EPG</translation>
    </message>
    <message>
        <source> + MM</source>
        <translation type='unfinished'>+ MM</translation>
    </message>
    <message>
        <source> + AFS</source>
        <translation type='unfinished'>+ AFS</translation>
    </message>
    <message>
        <source>Scanning...</source>
        <translation type='unfinished'>Rastreando...</translation>
    </message>
    <message>
        <source>&lt;b&gt;Text Message:&lt;/b&gt; On the top right the text message label is shown. This label only appears when an actual text message is transmitted. If the current service does not transmit a text message, the label will be disabled.</source>
        <translation type='unfinished'>&lt;b&gt;Mensaje de Texto:&lt;/b&gt; En la parte superior derecha se muestra la etiqueta del mensaje de texto. Esta etiqueta aparece sólo cuando se transmite un mensaje de texto real. Si el servicio actual no transmite un mensaje de texto, la etiqueta quedará deshabilitada.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Input Level:&lt;/b&gt; The input level meter shows the relative input signal peak level in dB. If the level is too high, the meter turns from green to red. The red region should be avoided since overload causes distortions which degrade the reception performance. Too low levels should be avoided too, since in this case the Signal-to-Noise Ratio (SNR) degrades.</source>
        <translation type='unfinished'>&lt;b&gt;Nivel de Entrada:&lt;/b&gt; El medidor del nivel de entrada muestra el valor de pico relativo (en dB) de la señal de entrada. Si el nivel es demasiado elevado, el medidor pasa de verde a rojo. La región roja debería evitarse, ya que la sobrecarga causa distorsiones que degradan el rendimiento de la recepción. Igualmente, deberían evitarse los niveles de entrada demasiado bajos, ya que en este caso la SNR (Relación Señal a Ruido) es la que se degrada.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Status LEDs:&lt;/b&gt; The three status LEDs show the current CRC status of the three logical channels of a DRM stream. These LEDs are the same as the top LEDs on the Evaluation Dialog.</source>
        <translation type='unfinished'>&lt;b&gt;LEDs indicadores de estado:&lt;/b&gt; Los tres LEDs indicadores de estado muestran el estado CRC (Cyclic Redundancy Check) de los tres canales lógicos de una trama DRM. Estos LEDs son los mismos que los LEDs de la parte superior de la ventana de Evaluación.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Station Label and Info Display:&lt;/b&gt; In the big label with the black background the station label and some other information about the current selected service is displayed. The magenta text on the top shows the bit-rate of the current selected service (The abbreviations EEP and UEP stand for Equal Error Protection and Unequal Error Protection. UEP is a feature of DRM for a graceful degradation of the decoded audio signal in case of a bad reception situation. UEP means that some parts of the audio is higher protected and some parts are lower protected (the ratio of higher protected part length to total length is shown in the brackets)), the audio compression format (e.g. AAC), if SBR is used and what audio mode is used (Mono, Stereo, P-Stereo -&gt; low-complexity or parametric stereo). In case SBR is used, the actual sample rate is twice the sample rate of the core AAC decoder. The next two types of information are the language and the program type of the service (e.g. German / News).&lt;br&gt;The big turquoise text in the middle is the station label. This label may appear later than the magenta text since this information is transmitted in a different logical channel of a DRM stream. On the right, the ID number connected with this service is shown.</source>
        <translation type='unfinished'>&lt;b&gt;Etiqueta de Estación y Visualización de Info:&lt;/b&gt; En el recuadro grande de fondo negro se puede visualizar la etiqueta de la estación recibida y otro tipo de información sobre el servicio actualmente seleccionado. El texto de color magenta de la parte superior muestra la tasa binaria del servicio elegido en se momento (EEP es un acrónimo para "Equal Error Protection", mientras que UEP lo es para "Unequal Error Protection". UEP es una característica de DRM que permite una elegante degradación de la señal de audio decodificada en caso de situación de mala recepción. UEP significa que algunas partes del audio están más protegidas y otras están menos protegidas (el ratio entre la longitud de la parte más protegida y la longitud total se muestra entre paréntesis)), el formato de compresión de audio (por ejemplo, AAC), si se usa SBR (Spectral Band Replication) y qué modo de audio está en uso (Mono, Estéreo, P-Estéreo -&gt; Estéreo Paramétrico o de complejidad baja). En caso de usarse SBR, la tasa de muestreo real es dos veces la tasa de muestreo del núcleo del decodificador AAC. La siguiente información que se muestra es el idioma y el tipo de programa del servicio (por ejemplo, Alemán / Noticias).&lt;br&gt;El texto grande del centro en color turquesa es la etiqueta de estación. Esta etiqueta puede aparecer más tarde que el texto en magenta, ya que esta información se transmite en un canal lógico diferente. A la derecha, se muestra el número de identidad (ID number) conectado a este servicio.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Service Selectors:&lt;/b&gt; In a DRM stream up to four services can be carried. The service can be an audio service, a data service or an audio service with data. Audio services can have associated text messages, in addition to any data component. If a Multimedia data service is selected, the Multimedia Dialog will automatically show up. On the right of each service selection button a short description of the service is shown. If an audio service has associated Multimedia data, "+ MM" is added to this text. If such a service is selected, opening the Multimedia Dialog will allow the data to be viewed while the audio is still playing. If the data component of a service is not Multimedia, but an EPG (Electronic Programme Guide) "+ EPG" is added to the description. The accumulated Programme Guides for all stations can be viewed by opening the Programme Guide Dialog. The selected channel in the Programme Guide Dialog defaults to the station being received. If Alternative Frequency Signalling is available, "+ AFS" is added to the description. In this case the alternative frequencies can be viewed by opening the Live Schedule Dialog.</source>
        <translation type='unfinished'>&lt;b&gt;Selectores de Servicio:&lt;/b&gt; Una trama DRM puede transportar hasta cuatro servicios. Puede tratarse de un servicio de audio, un servicio de datos o un servicio de audio con datos. Los servicios de audio pueden llevar asociados mensajes de texto, además de algún componente de datos. Si se elige un servicio de datos Multimedia, se mostrará automáticamente la ventana Multimedia. A la derecha de cada botón de selección de servicio se muestra una breve descripción del servicio. Si un servicio de audio tiene asociados datos Multimedia, se añade "+MM" a este texto. Si se elige dicho servicio, abrir la ventana Multimedia permitirá la visualización de los datos mientras que se sigue reproduciendo la señal de audio. Si la componente de datos del servicio no es Multimedia, pero se trata de una EPG (Guía Electrónica de Programa), se añade "+EPG" a la descripción. Las guías de programa acumuladas para todas las estaciones pueden verse abriendo la ventana de Guía de Programa. El canal seleccionado en dicha ventana es, por defecto, el de la estación que se está recibiendo. Si está disponible la Señalización de Frecuencia Alternativa (AFS), se añade "+AFS" a la descripción. En este caso, las frecuencias alternativas se pueden ver abriendo la ventana de Programación en Directo.</translation>
    </message>
    <message>
        <source>&amp;Multimedia settings...</source>
        <translation type='unfinished'>Opciones &amp;Multimedia...</translation>
    </message>
    <message>
        <source>Multimedia settings</source>
        <translation type='unfinished'>Opciones Multimedia</translation>
    </message>
    <message>
        <source>&amp;General settings...</source>
        <translation type='unfinished'>Opciones &amp;Generales...</translation>
    </message>
    <message>
        <source>General settings</source>
        <translation type='unfinished'>Opciones Generales</translation>
    </message>
</context>
<context>
    <name>FDRMDialogBase</name>
    <message>
        <source>Dream</source>
        <translation type='unfinished'>Dream</translation>
    </message>
    <message>
        <source>Level [dB]</source>
        <translation type='unfinished'>Nivel [dB]</translation>
    </message>
    <message>
        <source>15 kbps</source>
        <translation type='unfinished'>15 kbps</translation>
    </message>
    <message>
        <source>AAC+</source>
        <translation type='unfinished'>AAC+</translation>
    </message>
    <message>
        <source>Stereo</source>
        <translation type='unfinished'>Estéreo</translation>
    </message>
    <message>
        <source>Deutsche Welle</source>
        <translation type='unfinished'>Deutsche Welle</translation>
    </message>
    <message>
        <source>German</source>
        <translation type='unfinished'>Alemán</translation>
    </message>
    <message>
        <source>DE</source>
        <translation type='unfinished'>DE</translation>
    </message>
    <message>
        <source>Information</source>
        <translation type='unfinished'>Información</translation>
    </message>
    <message>
        <source>1001</source>
        <translation type='unfinished'>1001</translation>
    </message>
    <message>
        <source>TextTextMessage</source>
        <translation type='unfinished'>Mensaje_Texto</translation>
    </message>
    <message>
        <source>1</source>
        <comment>Selects the first service</comment>
        <translation type='unfinished'>1
Escoge el primer servicio</translation>
    </message>
    <message>
        <source>TextMiniService1</source>
        <translation type='unfinished'>MiniServicio1</translation>
    </message>
    <message>
        <source>2</source>
        <comment>Selects the second service</comment>
        <translation type='unfinished'>2
Escoge el segundo servicio</translation>
    </message>
    <message>
        <source>TextMiniService2</source>
        <translation type='unfinished'>MiniServicio2</translation>
    </message>
    <message>
        <source>3</source>
        <comment>Selects the third service</comment>
        <translation type='unfinished'>3
Escoge el tercer servicio</translation>
    </message>
    <message>
        <source>TextMiniService3</source>
        <translation type='unfinished'>MiniServicio3</translation>
    </message>
    <message>
        <source>4</source>
        <comment>Selects the fourth service</comment>
        <translation type='unfinished'>4
Escoge el cuarto servicio</translation>
    </message>
    <message>
        <source>TextMiniService4</source>
        <translation type='unfinished'>MiniServicio4</translation>
    </message>
</context>
<context>
    <name>GeneralSettingsDlg</name>
    <message>
        <source>Latitude value must be in the range 0 to 90</source>
        <translation type='unfinished'>El valor de latitud ha de estar en un rango de 0 a 90</translation>
    </message>
    <message>
        <source>Longitude value must be in the range 0 to 180</source>
        <translation type='unfinished'>El valor de longitud ha de estar en un rango de 0 a 90</translation>
    </message>
    <message>
        <source>Minutes value must be in the range 0 to 59</source>
        <translation type='unfinished'>El valor de los minutos debe estar en un rango de 0 a 59</translation>
    </message>
    <message>
        <source>Compile all fields on receiver coordinates</source>
        <translation type='unfinished'>Recopilar todos los datos en las coordenadas del receptor</translation>
    </message>
    <message>
        <source>&lt;b&gt;Receiver coordinates:&lt;/b&gt; Are used on Live Schedule Dialog for show a little green cube on the left of the target column if the receiver coordinates (latitude and longitude) are inside the target area of this transmission.&lt;br&gt;Receiver coordinates are also saved into the Log file.</source>
        <translation type='unfinished'>&lt;b&gt;Coordenadas del Receptor:&lt;/b&gt; Se usan en la ventana de Programación en Directo para mostrar un cubo verde pequeño a la izquierda de la columna de la columna del blanco si éstas (latitud y longitud) están dentro del área de blanco de esta transmisión.&lt;br&gt;Las coordenadas del receptor también se guardan en el archivo de registro.</translation>
    </message>
</context>
<context>
    <name>LiveScheduleDlg</name>
    <message>
        <source>System</source>
        <translation type='unfinished'>Sistema</translation>
    </message>
    <message>
        <source>Time [UTC]</source>
        <translation type='unfinished'>Hora [UTC]</translation>
    </message>
    <message>
        <source>Target</source>
        <translation type='unfinished'>Blanco</translation>
    </message>
    <message>
        <source>Show &amp;only active stations</source>
        <translation type='unfinished'>M&amp;ostrar sólo estaciones activas</translation>
    </message>
    <message>
        <source>Show &amp;all stations</source>
        <translation type='unfinished'>Mostrar tod&amp;as las estaciones</translation>
    </message>
    <message>
        <source>&amp;Disabled</source>
        <translation type='unfinished'>&amp;Deshabilitado</translation>
    </message>
    <message>
        <source>&amp;5 minutes</source>
        <translation type='unfinished'>&amp;5 minutos</translation>
    </message>
    <message>
        <source>&amp;15 minutes</source>
        <translation type='unfinished'>&amp;15 minutos</translation>
    </message>
    <message>
        <source>&amp;30 minutes</source>
        <translation type='unfinished'>&amp;30 minutos</translation>
    </message>
    <message>
        <source>Stations &amp;preview</source>
        <translation type='unfinished'>&amp;Previsualización de Estaciones</translation>
    </message>
    <message>
        <source>&amp;Save...</source>
        <translation type='unfinished'>&amp;Guardar...</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation type='unfinished'>&amp;Archivo</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation type='unfinished'>&amp;Ver</translation>
    </message>
    <message>
        <source>AFS Live Schedule</source>
        <translation type='unfinished'>Programación en Directo AFS</translation>
    </message>
    <message>
        <source>&lt;b&gt;UTC Time:&lt;/b&gt; Shows the current Coordinated Universal Time (UTC) which is also known as Greenwich Mean Time (GMT).</source>
        <translation type='unfinished'>&lt;b&gt;Hora UTC:&lt;/b&gt; Muestra la hora UTC (Coordinated Universal Time) actual, también conocida como GMT (Greenwich Mean Time).</translation>
    </message>
    <message>
        <source>Frequency [kHz/MHz]</source>
        <translation type='unfinished'>Frecuencia [kHz/MHz]</translation>
    </message>
    <message>
        <source>Start day</source>
        <translation type='unfinished'>Día de comienzo</translation>
    </message>
    <message>
        <source>Live Schedule</source>
        <translation type='unfinished'>Programación en Directo</translation>
    </message>
    <message>
        <source>&lt;b&gt;Freeze:&lt;/b&gt; If this check box is selected the live schedule is frozen.</source>
        <translation type='unfinished'>&lt;b&gt;Parar:&lt;/b&gt; Al seleccionar esta opción, la programación en directo se detiene.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Live Schedule List:&lt;/b&gt; In the live schedule list it&apos;s possible to view AFS (Alternative Frequency Signalling) informations trasmitted with the current DRM or AMSS signal.&lt;/b&gt;It is possible to show only active stations by changing a setting in the &apos;view&apos; menu.&lt;br&gt;The color of the cube on the left of the frequency shows the current status of the transmission.&lt;br&gt;A green box shows that the transmission takes place right now a red cube it is shown that the transmission is offline, a pink cube shown that the transmission soon will be offline.&lt;br&gt;If the stations preview is active an orange box shows the stations that will be active.&lt;br&gt;A little green cube on the left of the target column show that the receiver coordinates (latitude and longitude) stored into Dream settings are into the target area of this transmission.&lt;br&gt;The list can be sorted by clicking on the headline of the column.</source>
        <translation type='unfinished'>&lt;b&gt;Lista de Programación en Directo:&lt;/b&gt; En esta lista es posible ver informaciones AFS (Señalización de Frecuencia Alternativa) transmitidas en la señal DRM o AMSS actual. Es posible mostrar únicamente las estaciones activas cambiando una opción en el menú &apos;ver&apos;.&lt;br&gt;El color del cubo a la izquierda de la frecuencia muestra el estado actual de la transmisión.&lt;br&gt;Una caja verde significa que la transmisión tiene lugar ahora; una de color roja significa que la transmisión está fuera de línea; una de color rosa significa que la transmisión estará pronto fuera de línea.&lt;br&gt;Si se activa la previsualización de estaciones, una caja de color naranja muestra las estaciones que estarán activas.&lt;br&gt;Un pequeño cubo de color verde a la izquierda de la columna del blanco de una transmisión indica que las coordenadas del receptor (latitud y longitud)  almacenadas en las opciones de Dream están dentro del área del blanco de dicha transmisión.&lt;br&gt;La lista puede ordenarse haciendo click sobre el título de la columna.</translation>
    </message>
</context>
<context>
    <name>MultimediaDlg</name>
    <message>
        <source>C&amp;lear all</source>
        <translation type='unfinished'>&amp;Borrar todo</translation>
    </message>
    <message>
        <source>&amp;Save...</source>
        <translation type='unfinished'>&amp;Guardar...</translation>
    </message>
    <message>
        <source>Save &amp;all...</source>
        <translation type='unfinished'>Guardar &amp;todo...</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
    <message>
        <source>Set &amp;Font...</source>
        <translation type='unfinished'>Ajustar &amp;Fuente...</translation>
    </message>
    <message>
        <source>&amp;File</source>
        <translation type='unfinished'>&amp;Archivo</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation type='unfinished'>&amp;Opciones</translation>
    </message>
    <message>
        <source>MOT Broadcast Web Site</source>
        <translation type='unfinished'>Página Web de Difusión MOT</translation>
    </message>
    <message>
        <source>The home page is available</source>
        <translation type='unfinished'>La página Web está disponible</translation>
    </message>
    <message>
        <source>Image could not be loaded, </source>
        <translation type='unfinished'>No se pudo cargar la imagen,</translation>
    </message>
    <message>
        <source>-format not supported by this version of QT!</source>
        <translation type='unfinished'>-formato no soportado por esta versión de QT!</translation>
    </message>
    <message>
        <source>If you want to view the image, save it to file and use an external viewer</source>
        <translation type='unfinished'>Si desea visualizar esta imagen, guarde el fichero y utilice un visualizador externo</translation>
    </message>
    <message>
        <source>MOT Slideshow Viewer</source>
        <translation type='unfinished'>Exposición de Diapositivas MOT</translation>
    </message>
    <message>
        <source>No data service or data service not supported.</source>
        <translation type='unfinished'>Sin servicio de datos (o servicio de datos no soportado).</translation>
    </message>
    <message>
        <source>The homepage is available.</source>
        <translation type='unfinished'>La página Web está disponible.</translation>
    </message>
    <message>
        <source>Press the button to open it in the default browser.</source>
        <translation type='unfinished'>Presione el botón para abrirlo en el buscador por defecto.</translation>
    </message>
    <message>
        <source>Multimedia</source>
        <translation type='unfinished'>Multimedia</translation>
    </message>
</context>
<context>
    <name>MultimediaDlgBase</name>
    <message>
        <source>Dream Multimedia</source>
        <translation type='unfinished'>Dream Multimedia</translation>
    </message>
    <message>
        <source>Receiving Status:</source>
        <translation type='unfinished'>Estado de la Recepción:</translation>
    </message>
    <message>
        <source>0/0</source>
        <translation type='unfinished'>0/0</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <source>Sun</source>
        <translation type='unfinished'>Dom</translation>
    </message>
    <message>
        <source>Mon</source>
        <translation type='unfinished'>Lun</translation>
    </message>
    <message>
        <source>Tue</source>
        <translation type='unfinished'>Mar</translation>
    </message>
    <message>
        <source>Wed</source>
        <translation type='unfinished'>Mié</translation>
    </message>
    <message>
        <source>Thu</source>
        <translation type='unfinished'>Jue</translation>
    </message>
    <message>
        <source>Fri</source>
        <translation type='unfinished'>Vie</translation>
    </message>
    <message>
        <source>Sat</source>
        <translation type='unfinished'>Sáb</translation>
    </message>
    <message>
        <source>irregular</source>
        <translation type='unfinished'>irregular</translation>
    </message>
    <message>
        <source>daily</source>
        <translation type='unfinished'>diario</translation>
    </message>
    <message>
        <source>from Sun to Thu</source>
        <translation type='unfinished'>de Dom a Jue</translation>
    </message>
    <message>
        <source>from Sun to Fri</source>
        <translation type='unfinished'>de Dom a Vie</translation>
    </message>
    <message>
        <source>from Mon to Fri</source>
        <translation type='unfinished'>de Lun a Vie</translation>
    </message>
    <message>
        <source>from Mon to Sat</source>
        <translation type='unfinished'>de Lun a Sáb</translation>
    </message>
</context>
<context>
    <name>StationsDlg</name>
    <message>
        <source>Station Name</source>
        <translation type='unfinished'>Nombre de Estación</translation>
    </message>
    <message>
        <source>Time [UTC]</source>
        <translation type='unfinished'>Hora [UTC]</translation>
    </message>
    <message>
        <source>Frequency [kHz]</source>
        <translation type='unfinished'>Frecuencia [kHz]</translation>
    </message>
    <message>
        <source>Target</source>
        <translation type='unfinished'>Blanco</translation>
    </message>
    <message>
        <source>Power [kW]</source>
        <translation type='unfinished'>Potencia [kW]</translation>
    </message>
    <message>
        <source>Country</source>
        <translation type='unfinished'>País</translation>
    </message>
    <message>
        <source>Site</source>
        <translation type='unfinished'>Emplazamiento</translation>
    </message>
    <message>
        <source>Language</source>
        <translation type='unfinished'>Idioma</translation>
    </message>
    <message>
        <source>Days</source>
        <translation type='unfinished'>Días</translation>
    </message>
    <message>
        <source>Show &amp;only active stations</source>
        <translation type='unfinished'>Mostrar sól&amp;o estaciones activas</translation>
    </message>
    <message>
        <source>Show &amp;all stations</source>
        <translation type='unfinished'>Mostrar tod&amp;as las estaciones</translation>
    </message>
    <message>
        <source>&amp;Disabled</source>
        <translation type='unfinished'>&amp;Deshabilitado</translation>
    </message>
    <message>
        <source>&amp;5 minutes</source>
        <translation type='unfinished'>&amp;5 minutos</translation>
    </message>
    <message>
        <source>&amp;15 minutes</source>
        <translation type='unfinished'>&amp;15 minutos</translation>
    </message>
    <message>
        <source>&amp;30 minutes</source>
        <translation type='unfinished'>&amp;30 minutos</translation>
    </message>
    <message>
        <source>Stations &amp;preview</source>
        <translation type='unfinished'>&amp;Previsualización de Estaciones</translation>
    </message>
    <message>
        <source>None</source>
        <translation type='unfinished'>Ninguna</translation>
    </message>
    <message>
        <source>Other</source>
        <translation type='unfinished'>Otra</translation>
    </message>
    <message>
        <source>Enable S-Meter</source>
        <translation type='unfinished'>Habilitar Parámetros S</translation>
    </message>
    <message>
        <source>With DRM Modification</source>
        <translation type='unfinished'>Con Modificación DRM</translation>
    </message>
    <message>
        <source>&amp;Get Update...</source>
        <translation type='unfinished'>Actuali&amp;zar...</translation>
    </message>
    <message>
        <source>&amp;View</source>
        <translation type='unfinished'>&amp;Ver</translation>
    </message>
    <message>
        <source>&amp;Remote</source>
        <translation type='unfinished'>&amp;Remoto</translation>
    </message>
    <message>
        <source>&amp;Update</source>
        <translation type='unfinished'>&amp;Actualización</translation>
    </message>
    <message>
        <source>Dream Schedule Update</source>
        <translation type='unfinished'>Actualización de Programación</translation>
    </message>
    <message>
        <source>Dream tries to download the newest DRM schedule
from www.drm-dx.de (powered by Klaus Schneider).
Your computer must be connected to the internet.

The current file DRMSchedule.ini will be overwritten.
Do you want to continue?</source>
        <translation type='unfinished'>Dream intentará descargar la programación DRM más reciente
del sitio web www.drm-dx.de (impulsada por Klaus Schneider).
Su ordenador ha de estar conectado a Internet.

La versión actual de DRMSchedule.ini se sobrescribirá.
Desea continuar?</translation>
    </message>
    <message>
        <source>- the file &apos;DRMSchedule.ini&apos; could not be written</source>
        <translation type='unfinished'>- no se pudo escribir el archivo &apos;DRMSchedule.ini&apos;</translation>
    </message>
    <message>
        <source>Update successful.
Due to network problems with the Windows version of QT, the Dream software must be restarted after a DRMSchedule update.
Please exit Dream now.</source>
        <translation type='unfinished'>Actualización llevada a cabo con éxito.
Debido a problemas de red con la versión Windows de QT, Dream ha de reiniciarse después de una actualización de la programación.
Por favor, salga de Dream ahora.</translation>
    </message>
    <message>
        <source>Ok</source>
        <translation type='unfinished'>Ok</translation>
    </message>
    <message>
        <source>Update successful.</source>
        <translation type='unfinished'>Actualización llevada a cabo con éxito.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Frequency Counter:&lt;/b&gt; The current frequency value can be changed by using this counter. The tuning steps are 100 kHz for the  buttons with three arrows, 10 kHz for the buttons with two arrows and 1 kHz for the buttons having only one arrow. By keeping the button pressed, the values are increased / decreased automatically.</source>
        <translation type='unfinished'>&lt;b&gt;Contador de Frecuencia:&lt;/b&gt; El valor de la frecuencia actual se puede cambiar usando este contador. Los botones con tres flechas producen un incremento/decremento de 100 kHz; los de dos flechas, de 10 kHz y los de una flecha, de 1 kHz. Al mantener pulsado un botón, los valores se incrementan/decrementan de forma automática.</translation>
    </message>
    <message>
        <source>&lt;b&gt;UTC Time:&lt;/b&gt; Shows the current Coordinated Universal Time (UTC) which is also known as Greenwich Mean Time (GMT).</source>
        <translation type='unfinished'>&lt;b&gt;Hora UTC:&lt;/b&gt; Muestra la hora UTC (Coordinated Universal Time) actual, también conocida como GMT (Greenwich Mean Time).</translation>
    </message>
    <message>
        <source>&lt;b&gt;Signal-Meter:&lt;/b&gt; Shows the signal strength level in dB relative to S9.&lt;br&gt;Note that not all front-ends controlled by hamlib support this feature. If the s-meter is not available, the controls are disabled.</source>
        <translation type='unfinished'>&lt;b&gt;Medidor de señal:&lt;/b&gt; Muestra en dB el nivel de fuerza de la señal relativo a S9.&lt;br&gt;Obsérvese que no todos los receptores controlados por Hamlib soportan esta opción. Si el medidor de parámetros S no está disponible, se deshabilitan los controles.</translation>
    </message>
    <message>
        <source> (last update: </source>
        <translation type='unfinished'>(última actualización:</translation>
    </message>
    <message>
        <source>&amp;Get Update</source>
        <translation type='unfinished'>Actuali&amp;zar</translation>
    </message>
    <message>
        <source>&lt;b&gt;Stations List:&lt;/b&gt; In the stations list view all DRM stations which are stored in the DRMSchedule.ini file are shown. It is possible to show only active stations by changing a setting in the &apos;view&apos; menu. The color of the cube on the left of a menu item shows the current status of the DRM transmission. A green box shows that the transmission takes place right now, a yellow cube shows that this is a test transmission and with a red cube it is shown that the transmission is offline, a pink cube shown that the transmission soon will be offline.&lt;br&gt;If the stations preview is active an orange box shows the stations that will be active.&lt;br&gt;The list can be sorted by clicking on the headline of the column.&lt;br&gt;By clicking on a menu item, a remote front-end can be automatically switched to the current frequency and the Dream software is reset to a new acquisition (to speed up the synchronization process). Also, the log-file frequency edit is automatically updated.</source>
        <translation type='unfinished'>&lt;b&gt;Lista de Estaciones:&lt;/b&gt; En la lista de estaciones se muestran todas las estaciones guardadas en el archivo DRMSchedule.ini. Es posible mostrar únicamente las estaciones activas cambiando una opción en el menú &apos;ver&apos;. El color del cubo a la izquierda de la frecuencia muestra el estado actual de la transmisión. Una caja verde significa que la transmisión tiene lugar ahora; una de color roja significa que la transmisión está fuera de línea; una de color rosa significa que la transmisión estará pronto fuera de línea.&lt;br&gt;Si se activa la previsualización de estaciones, una caja de color naranja muestra las estaciones que estarán activas.&lt;br&gt;La lista puede ordenarse haciendo click sobre el título de la columna.&lt;br&gt;Al hacer click sobre un elemento del menú, el receptor (en caso de soportarlo Hamlib) puede sintonizarse automáticamente a la frecuencia correspondiente y el software Dream inicia una nueva adquisición (para acelerar el proceso de sincronización). También se actualiza automáticamente la frecuencia del archivo de registro.</translation>
    </message>
</context>
<context>
    <name>TransmDialog</name>
    <message>
        <source>new</source>
        <translation type='unfinished'>nuevo</translation>
    </message>
    <message>
        <source>&amp;Start</source>
        <translation type='unfinished'>&amp;Comenzar</translation>
    </message>
    <message>
        <source>2 s (Long Interleaving)</source>
        <translation type='unfinished'>2 s (Intercalado largo)</translation>
    </message>
    <message>
        <source>400 ms (Short Interleaving)</source>
        <translation type='unfinished'>400 ms (Intercalado corto)</translation>
    </message>
    <message>
        <source>SM 16-QAM</source>
        <translation type='unfinished'>SM 16-QAM</translation>
    </message>
    <message>
        <source>SM 64-QAM</source>
        <translation type='unfinished'>SM 64-QAM</translation>
    </message>
    <message>
        <source>4-QAM</source>
        <translation type='unfinished'>4-QAM</translation>
    </message>
    <message>
        <source>16-QAM</source>
        <translation type='unfinished'>16-QAM</translation>
    </message>
    <message>
        <source>
This is a test transmission</source>
        <translation type='unfinished'>Esto es una transmisión de prueba</translation>
    </message>
    <message>
        <source>&amp;Sound Card Selection</source>
        <translation type='unfinished'>Elegir Tarjeta de &amp;Sonido</translation>
    </message>
    <message>
        <source>&amp;Settings</source>
        <translation type='unfinished'>&amp;Opciones</translation>
    </message>
    <message>
        <source>&amp;?</source>
        <translation type='unfinished'>&amp;?</translation>
    </message>
    <message>
        <source>&amp;Stop</source>
        <translation type='unfinished'>&amp;Parar</translation>
    </message>
    <message>
        <source>Image Files (*.png *.jpg *.jpeg *.jfif)</source>
        <translation type='unfinished'>Archivos de imagen (*.png *.jpg *.jpeg *.jfif)</translation>
    </message>
    <message>
        <source>&lt;b&gt;Dream Logo:&lt;/b&gt; This is the official logo of the Dream software.</source>
        <translation type='unfinished'>&lt;b&gt; Logotipo de Dream&lt;/b&gt;: Este es el logotipo oficial del SW Dream</translation>
    </message>
    <message>
        <source>&lt;b&gt;Input Level:&lt;/b&gt; The input level meter shows the relative input signal peak level in dB. If the level is too high, the meter turns from green to red.</source>
        <translation type='unfinished'>&lt;b&gt;Nivel de Entrada:&lt;/b&gt; El medidor del nivel de entrada muestra el valor de pico relativo (en dB) de la señal de entrada. Si el nivel es demasiado elevado, el medidor pasa de verde a rojo.</translation>
    </message>
    <message>
        <source>&lt;b&gt;DRM Robustness Mode:&lt;/b&gt; In a DRM system, four possible robustness modes are defined to adapt the system to different channel conditions. According to the DRM standard:&lt;ul&gt;&lt;li&gt;&lt;i&gt;Mode A:&lt;/i&gt; Gaussian channels, with minor fading&lt;/li&gt;&lt;li&gt;&lt;i&gt;Mode B:&lt;/i&gt; Time and frequency selective channels, with longer delay spread&lt;/li&gt;&lt;li&gt;&lt;i&gt;Mode C:&lt;/i&gt; As robustness mode B, but with higher Doppler spread&lt;/li&gt;&lt;li&gt;&lt;i&gt;Mode D:&lt;/i&gt; As robustness mode B, but with severe delay and Doppler spread&lt;/li&gt;&lt;/ul&gt;</source>
        <translation type='unfinished'>&lt;b&gt;Modo de Robustez DRM:&lt;/b&gt; En un sistema DRM, se definen cuatro modos de robustez posibles para adaptar el sistema a las diferentes condiciones del canal. Según el estándar de DRM:&lt;ul&gt;&lt;li&gt;&lt;i&gt;Modo A:&lt;/i&gt; Canales Gaussianos, con escaso desvanecimiento&lt;/li&gt;&lt;li&gt;&lt;i&gt;Modo B:&lt;/i&gt; Canales Selectivos en Tempo y en Frecuencia, con mayor propagación de retardo&lt;/li&gt;&lt;li&gt;&lt;i&gt;Modo C:&lt;/i&gt; Como el modo de robustez B, pero con mayor propagación Doppler&lt;/li&gt;&lt;li&gt;&lt;i&gt;Modo D:&lt;/i&gt; Como el modo de robustez C, pero con retardo severo&lt;/li&gt;&lt;/ul&gt;</translation>
    </message>
    <message>
        <source>&lt;b&gt;DRM Bandwidth:&lt;/b&gt; The bandwith is the gross bandwidth of the generated DRM signal. Not all DRM robustness mode / bandwidth constellations are possible, e.g., DRM robustness mode D and C are only defined for the bandwidths 10 kHz and 20 kHz.</source>
        <translation type='unfinished'>&lt;b&gt;Ancho de Banda DRM:&lt;/b&gt; Es el ancho de banda bruto de la señal DRM generada. No todas las conjugaciones modo de robustez/ancho de banda son posibles. Por ejemplo, los modos de robustez C y D sólo están definidos para anchos de banda de 10 kHz y 20 kHz.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Output intermediate frequency of DRM signal:&lt;/b&gt; Set the output intermediate frequency (IF) of generated DRM signal in the &apos;sound-card pass-band&apos;. In some DRM modes, the IF is located at the edge of the DRM signal, in other modes it is centered. The IF should be chosen that the DRM signal lies entirely inside the sound-card bandwidth.</source>
        <translation type='unfinished'>&lt;b&gt;Frecuencia Intermedia de salida de la señal DRM:&lt;/b&gt; Ajusta la frecuencia intermedia (IF) de salida de la señal DRM generada en el filtro paso banda de la tarjeta de sonido. En algunos modos DRM, la IF está en un extremo de la señal DRM, mientras que en otros está centrada. La IF se ha de escoger de modo tal que la señal DRM entre íntegramente en el ancho de banda de la tarjeta de sonido.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Output format:&lt;/b&gt; Since the sound-card outputs signals in stereo format, it is possible to output the DRM signal in three formats:&lt;ul&gt;&lt;li&gt;&lt;b&gt;real valued&lt;/b&gt; output on both, left and right, sound-card channels&lt;/li&gt;&lt;li&gt;&lt;b&gt;I / Q&lt;/b&gt; output which is the in-phase and quadrature component of the complex base-band signal at the desired IF. In-phase is output on the left channel and quadrature on the right channel.&lt;/li&gt;&lt;li&gt;&lt;b&gt;E / P&lt;/b&gt; output which is the envelope and phase on separate channels. This output type cannot be used if the Dream transmitter is regularly compiled with a sound-card sample rate of 48 kHz since the spectrum of these components exceed the bandwidth of 20 kHz.&lt;br&gt;The envelope signal is output on the left channel and the phase is output on the right channel.&lt;/li&gt;&lt;/ul&gt;</source>
        <translation type='unfinished'>&lt;b&gt;Formato de salida:&lt;/b&gt; Dado que la tarjeta de sonido saca las señales en formato estéreo, es posible sacar la señal DRM en tres formatos:&lt;ul&gt;&lt;li&gt;&lt;b&gt;Valor real&lt;/b&gt; Salida en ambos canales de la tarjeta de sonido, izquierdo y derecho&lt;/li&gt;&lt;li&gt;&lt;b&gt;I / Q&lt;/b&gt; Saca las componentes en fase y en cuadratura de la señal compleja en banda base a la frecuencia intermedia deseada. La componente en fase se saca por el canal izquierdo y la componente en cuadratura por el derecho&lt;/li&gt;&lt;li&gt;&lt;b&gt;E / P&lt;/b&gt; Saca la envolvente y la fase en diferentes canales. Este tipo de salida no se puede emplear si el transmisor Dream está compilado regularmente con una tasa de muestreo en la tarjeta de sonido de 48 kHz, ya que el espectro de estas componentes excede el ancho de banda de 20 kHz.&lt;br&gt;La señal envolvente se saca por el canal izquierdo y la de fase por el canal derecho&lt;/li&gt;&lt;ul&gt;</translation>
    </message>
    <message>
        <source>&lt;b&gt;MSC interleaver mode:&lt;/b&gt; The symbol interleaver depth can be either short (approx. 400 ms) or long (approx. 2 s). The longer the interleaver the better the channel decoder can correct errors from slow fading signals. But the longer the interleaver length the longer the delay until (after a re-synchronization) audio can be heard.</source>
        <translation type='unfinished'>&lt;b&gt;Modo de Intercalado MSC:&lt;/b&gt; La profundidad del intercalado de símbolos puede ser bien corta (aproximadamente 400 ms) o bien larga (aproximadamente 2 s). Mientras mayor sea la longitud de intercalado, mejor podrá el decodificador de canal corregir errores en señales con escaso desvanecimiento, aunque también será mayor el retardo (después de una resincronización) hasta que pueda oirse el audio.</translation>
    </message>
    <message>
        <source>Dream DRM Transmitter</source>
        <translation type='unfinished'>Transmisor DRM Dream</translation>
    </message>
</context>
<context>
    <name>TransmDlgBase</name>
    <message>
        <source>Dream DRM Transmitter</source>
        <translation type='unfinished'>Transmisor DRM Dream</translation>
    </message>
    <message>
        <source>Volker Fischer, Alexander Kurpiers</source>
        <translation type='unfinished'>Volker Fischer, Alexander Kurpiers</translation>
    </message>
    <message>
        <source>Darmstadt University of Technology
Institute for Communication Technology</source>
        <translation type='unfinished'>Darmstadt University of Technology
Institute for Communication Technology</translation>
    </message>
    <message>
        <source>Indicators</source>
        <translation type='unfinished'>Indicadores</translation>
    </message>
    <message>
        <source>Audio Level [dB]:</source>
        <translation type='unfinished'>Nivel de Audio [dB]:</translation>
    </message>
    <message>
        <source>TextLabelCurPict</source>
        <translation type='unfinished'>EtiquetaFoto</translation>
    </message>
    <message>
        <source>Channel Parameters</source>
        <translation type='unfinished'>Parámetros del Canal</translation>
    </message>
    <message>
        <source>Robustness Mode</source>
        <translation type='unfinished'>Modo de Robustez</translation>
    </message>
    <message>
        <source>A</source>
        <translation type='unfinished'>A</translation>
    </message>
    <message>
        <source>B</source>
        <translation type='unfinished'>B</translation>
    </message>
    <message>
        <source>C</source>
        <translation type='unfinished'>C</translation>
    </message>
    <message>
        <source>D</source>
        <translation type='unfinished'>D</translation>
    </message>
    <message>
        <source>Bandwidth</source>
        <translation type='unfinished'>Ancho de Banda</translation>
    </message>
    <message>
        <source>4,5 kHz</source>
        <translation type='unfinished'>4,5 kHz</translation>
    </message>
    <message>
        <source>9 kHz</source>
        <translation type='unfinished'>9 kHz</translation>
    </message>
    <message>
        <source>18 kHz</source>
        <translation type='unfinished'>18 kHz</translation>
    </message>
    <message>
        <source>5 kHz</source>
        <translation type='unfinished'>5 kHz</translation>
    </message>
    <message>
        <source>10 kHz</source>
        <translation type='unfinished'>10 kHz</translation>
    </message>
    <message>
        <source>20 kHz</source>
        <translation type='unfinished'>20 kHz</translation>
    </message>
    <message>
        <source>MSC Constellation Scheme:</source>
        <translation type='unfinished'>Esquema de Constelación MSC:</translation>
    </message>
    <message>
        <source>MSC Protection Level:</source>
        <translation type='unfinished'>Nivel de Protección MSC:</translation>
    </message>
    <message>
        <source>SDC Constellation Scheme:</source>
        <translation type='unfinished'>Esquema de Constelación SDC:</translation>
    </message>
    <message>
        <source>MSC Interleaver Mode:</source>
        <translation type='unfinished'>Modo de Intercalado MSC:</translation>
    </message>
    <message>
        <source>Output</source>
        <translation type='unfinished'>Salida</translation>
    </message>
    <message>
        <source>IF:</source>
        <translation type='unfinished'>Freq. Interm:</translation>
    </message>
    <message>
        <source> Hz</source>
        <translation type='unfinished'>Hz</translation>
    </message>
    <message>
        <source>Real val.</source>
        <translation type='unfinished'>Val. Real</translation>
    </message>
    <message>
        <source>I / Q (pos)</source>
        <translation type='unfinished'>I / Q (pos)</translation>
    </message>
    <message>
        <source>I / Q (neg)</source>
        <translation type='unfinished'>I / Q (neg)</translation>
    </message>
    <message>
        <source>E / P</source>
        <translation type='unfinished'>E / P</translation>
    </message>
    <message>
        <source>Service 1</source>
        <translation type='unfinished'>Servicio 1</translation>
    </message>
    <message>
        <source>Enable</source>
        <translation type='unfinished'>Habilitar</translation>
    </message>
    <message>
        <source>Label:</source>
        <translation type='unfinished'>Etiqueta:</translation>
    </message>
    <message>
        <source>Service ID:</source>
        <translation type='unfinished'>Id. de Servicio:</translation>
    </message>
    <message>
        <source>Language:</source>
        <translation type='unfinished'>Idioma:</translation>
    </message>
    <message>
        <source>Audio</source>
        <translation type='unfinished'>Audio</translation>
    </message>
    <message>
        <source>Program Type:</source>
        <translation type='unfinished'>Tipo de programa:</translation>
    </message>
    <message>
        <source>Text Message</source>
        <translation type='unfinished'>Mensaje de texto:</translation>
    </message>
    <message>
        <source>&amp;Clear All</source>
        <translation type='unfinished'>&amp;Borrar todo</translation>
    </message>
    <message>
        <source>&amp;Add / Modify</source>
        <translation type='unfinished'>&amp;Añadir / Modificar</translation>
    </message>
    <message>
        <source>Data (SlideShow Application)</source>
        <translation type='unfinished'>Datos (Aplicación Diapositivas)</translation>
    </message>
    <message>
        <source>Column 1</source>
        <translation type='unfinished'>Columna 1</translation>
    </message>
    <message>
        <source>New Item</source>
        <translation type='unfinished'>Nuevo objeto</translation>
    </message>
    <message>
        <source>&amp;Add</source>
        <translation type='unfinished'>&amp;Añadir</translation>
    </message>
    <message>
        <source>Service 2</source>
        <translation type='unfinished'>Servicio 2</translation>
    </message>
    <message>
        <source>Service 3</source>
        <translation type='unfinished'>Servicio 3</translation>
    </message>
    <message>
        <source>Service 4</source>
        <translation type='unfinished'>Servicio 4</translation>
    </message>
    <message>
        <source>&amp;Start</source>
        <translation type='unfinished'>Co&amp;menzar</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
</context>
<context>
    <name>systemevalDlg</name>
    <message>
        <source>MLC: Number of Iterations: </source>
        <translation type='unfinished'>MLC: Nº de Iteraciones:</translation>
    </message>
    <message>
        <source>History</source>
        <translation type='unfinished'>Historial</translation>
    </message>
    <message>
        <source>Constellation</source>
        <translation type='unfinished'>Constelación</translation>
    </message>
    <message>
        <source>Channel</source>
        <translation type='unfinished'>Canal</translation>
    </message>
    <message>
        <source>Spectrum</source>
        <translation type='unfinished'>Espectro</translation>
    </message>
    <message>
        <source>SNR Spectrum</source>
        <translation type='unfinished'>Espectro SNR</translation>
    </message>
    <message>
        <source>Audio Spectrum</source>
        <translation type='unfinished'>Espectro Audio</translation>
    </message>
    <message>
        <source>Shifted PSD</source>
        <translation type='unfinished'>DEP Desplazada</translation>
    </message>
    <message>
        <source>Waterfall Input Spectrum</source>
        <translation type='unfinished'>Espectro de Entrada en Cascada</translation>
    </message>
    <message>
        <source>Input Spectrum</source>
        <translation type='unfinished'>Espectro de Entrada</translation>
    </message>
    <message>
        <source>Input PSD</source>
        <translation type='unfinished'>DEP de Entrada</translation>
    </message>
    <message>
        <source>MSC</source>
        <translation type='unfinished'>MSC</translation>
    </message>
    <message>
        <source>SDC</source>
        <translation type='unfinished'>SDC</translation>
    </message>
    <message>
        <source>FAC</source>
        <translation type='unfinished'>FAC</translation>
    </message>
    <message>
        <source>FAC / SDC / MSC</source>
        <translation type='unfinished'>FAC / SDC / MSC</translation>
    </message>
    <message>
        <source>Frequency / Sample Rate</source>
        <translation type='unfinished'>Frecuencia / Tasa de Muestreo</translation>
    </message>
    <message>
        <source>Delay / Doppler</source>
        <translation type='unfinished'>Retardo / Doppler</translation>
    </message>
    <message>
        <source>SNR / Audio</source>
        <translation type='unfinished'>SNR / Audio</translation>
    </message>
    <message>
        <source>Transfer Function</source>
        <translation type='unfinished'>Función de Transferencia</translation>
    </message>
    <message>
        <source>Impulse Response</source>
        <translation type='unfinished'>Respuesta Impulsional</translation>
    </message>
    <message>
        <source>ListView context menu</source>
        <translation type='unfinished'>Menú Contextual ListView</translation>
    </message>
    <message>
        <source>&amp;Open in separate window</source>
        <translation type='unfinished'>&amp;Abrir en una ventana nueva</translation>
    </message>
    <message>
        <source>Chart Window</source>
        <translation type='unfinished'>Gráfico</translation>
    </message>
    <message>
        <source>Metrics [dB]: MSC: </source>
        <translation type='unfinished'>Medidas [dB]: MSC:</translation>
    </message>
    <message>
        <source>2 s (Long Interleaving)</source>
        <translation type='unfinished'>2 s (Intercalado largo)</translation>
    </message>
    <message>
        <source>400 ms (Short Interleaving)</source>
        <translation type='unfinished'>400 ms (Intercalado corto)</translation>
    </message>
    <message>
        <source>Interleaver Depth:</source>
        <translation type='unfinished'>Profundidad de Intercalado:</translation>
    </message>
    <message>
        <source>SDC / MSC Mode:</source>
        <translation type='unfinished'>Modo SDC / MSC:</translation>
    </message>
    <message>
        <source>Prot. Level (B / A):</source>
        <translation type='unfinished'>Nivel de Protección (B / A):</translation>
    </message>
    <message>
        <source>Audio: </source>
        <translation type='unfinished'>Audio:</translation>
    </message>
    <message>
        <source> / Data: </source>
        <translation type='unfinished'>/ Datos:</translation>
    </message>
    <message>
        <source>Number of Services:</source>
        <translation type='unfinished'>Nº de Servicios:</translation>
    </message>
    <message>
        <source>Service not available</source>
        <translation type='unfinished'>Servicio no disponible</translation>
    </message>
    <message>
        <source>Received time - date:</source>
        <translation type='unfinished'>Hora / Fecha Recibidas:</translation>
    </message>
    <message>
        <source>DreamOut.wav</source>
        <translation type='unfinished'>DreamOut.wav</translation>
    </message>
    <message>
        <source>Label            </source>
        <translation type='unfinished'>Etiqueta</translation>
    </message>
    <message>
        <source>
Bitrate          </source>
        <translation type='unfinished'>Tasa de Bit</translation>
    </message>
    <message>
        <source>
Mode             </source>
        <translation type='unfinished'>Modo</translation>
    </message>
    <message>
        <source>
Bandwidth        </source>
        <translation type='unfinished'>Ancho de Banda</translation>
    </message>
    <message>
        <source>&lt;b&gt;DC Frequency Offset:&lt;/b&gt; This is the estimation of the DC frequency offset. This offset corresponds to the resulting sound card intermedia frequency of the front-end. This frequency is not restricted to a certain value. The only restriction is that the DRM spectrum must be completely inside the bandwidth of the sound card.</source>
        <translation type='unfinished'>&lt;b&gt;Offset de Frecuencia DC:&lt;/b&gt; Es una estimación del offset de la frecuencia continua (DC). Este offset corresponde a la frecuencia intermedia de la tarjeta de sonido resultante en el receptor. Esta frecuencia no está restringida a un valor determinado. La única restricción es que el espectro DRM ha de estar completamente dentro del ancho de banda de la tarjeta de sonido.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Sample Frequency Offset:&lt;/b&gt; This is the estimation of the sample rate offset between the sound card sample rate of the local computer and the sample rate of the D / A (digital to analog) converter in the transmitter. Usually the sample rate offset is very constant for a given sound card. Therefore it is useful to inform the Dream software about this value at application startup to increase the acquisition speed and reliability.</source>
        <translation type='unfinished'>&lt;b&gt;Offset de Frecuencia de Muestreo:&lt;/b&gt; Se trata de una estimación del offset de la tasa de muestreo entre la tasa de muestreo de la tarjeta de sonido del ordenador local y la tasa de muestreo de conversor digital / analógico (DAC) del transmisor. Normalmente, este offset es constante para una cierta tarjeta de sonido. Por tanto, resulta útil informar con este valor a Dream en el arranque de la aplicación para aumentar la velocidad de adquisición y la fiabilidad.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Doppler / Delay:&lt;/b&gt; The Doppler frequency of the channel is estimated for the Wiener filter design of channel estimation in time direction. If linear interpolation is set for channel estimation in time direction, this estimation is not updated. The Doppler frequency is an indication of how fast the channel varies with time. The higher the frequency, the faster the channel changes are.&lt;br&gt;The total delay of the Power Delay Spectrum (PDS) is estimated from the impulse response estimation derived from the channel estimation. This delay corresponds to the range between the two vertical dashed black lines in the Impulse Response (IR) plot.</source>
        <translation type='unfinished'>&lt;b&gt;Doppler / Retardo:&lt;/b&gt; La frecuencia Doppler del canal se estima con objeto de diseñar el filtro de Wiener para la estimación del canal en el dominio del tiempo. Si se configura la interpolación lineal para la estimación del canal en el dominio del tiempo, esta estimación no se actualiza. La frecuencia Doppler indica cómo de rápido varía el canal con el tiempo. Mientras mayor sea la frecuencia, más rápidamente se producen los cambios en el canal.&lt;br&gt;El retardo total del espectro de retardo de potencia (PDS - Power Delay Spectrum) se estima a través de la estimación de la respuesta impulsional derivada de la estimación del canal. Este retardo se corresponde con el rango entre las dos líneas negras verticales discontinuas en el gráfico de la respuesta impulsional (IR).</translation>
    </message>
    <message>
        <source>&lt;b&gt;I / O Interface LED:&lt;/b&gt; This LED shows the current status of the sound card interface. The yellow light shows that the audio output was corrected. Since the sample rate of the transmitter and local computer are different, from time to time the audio buffers will overflow or under run and a correction is necessary. When a correction occurs, a "click" sound can be heard. The red light shows that a buffer was lost in the sound card input stream. This can happen if a thread with a higher priority is at 100% and the Dream software cannot read the provided blocks fast enough. In this case, the Dream software will instantly loose the synchronization and has to re-synchronize. Another reason for red light is that the processor is too slow for running the Dream software.</source>
        <translation type='unfinished'>&lt;b&gt;LED Interfaz E / S:&lt;/b&gt; Este LED muestra el estado actual del interfaz de la tarjeta de sonido. La luz amarilla indica que la salida de audio fue corregida. Dado que las tasas de muestreo del transmisor y del ordenador local son diferentes, de vez en cuando los buffers de audio se saturarán o estarán infrautilizados, por lo que será necesaria una corrección. Cuando se produce una corrección, puede oírse un "click". La luz roja indica que un buffer se perdió en el flujo de entrada de la tarjeta de sonido. Esto puede ocurrir si un hilo de mayor prioridad está al 100% y Dream, por tanto, no puede leer lo suficientemente rápido los bloques provistos. En este caso, Dream perderá por un instante la sincronización y deberá resincronizarse. Otra razón por la que la luz roja puede aparecer es que el procesador es demasiado lento para ejecutar el software de recepción.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Time Sync Acq LED:&lt;/b&gt; This LED shows the state of the timing acquisition (search for the beginning of an OFDM symbol). If the acquisition is done, this LED will stay green.</source>
        <translation type='unfinished'>&lt;b&gt;LED de Adquisición de Sincronización Temporal:&lt;/b&gt; Este LED muestra el estado de la sincronización temporal (busca el principio de un símbolo OFDM). Una vez hecha la adquisición, la luz se muestra de color verde.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Frame Sync LED:&lt;/b&gt; The DRM frame synchronization status is shown with this LED. This LED is also only active during acquisition state of the Dream receiver. In tracking mode, this LED is always green.</source>
        <translation type='unfinished'>&lt;b&gt;LED de Sincronización en Frecuencia:&lt;/b&gt; El estado de sincronización de la trama DRM se muestra mediante este LED, que únicamente está activo durante el estado de adquisición del receptor Dream. En modo seguimiento, este LED está siempre verde.</translation>
    </message>
    <message>
        <source>&lt;b&gt;FAC CRC LED:&lt;/b&gt; This LED shows the Cyclic Redundancy Check (CRC) of the Fast Access Channel (FAC) of DRM. FAC is one of the three logical channels and is always modulated with a 4-QAM. If the FAC CRC check was successful, the receiver changes to tracking mode. The FAC LED is the indication whether the receiver is synchronized to a DRM transmission or not.&lt;br&gt;The bandwidth of the DRM signal, the constellation scheme of MSC and SDC channels and the interleaver depth are some of the parameters which are provided by the FAC.</source>
        <translation type='unfinished'>&lt;b&gt;LED de CRC de canal FAC:&lt;/b&gt; Este LED indica el CRC (Cyclic Redundancy Check) del canal FAC (Fast Access Channel) de DRM. FAC es uno de los tres canales lógicos y siempre va modulado con una 4-QAM. Si el CRC de este canal es correcto, el receptor cambia a modo seguimiento. El LED del canal FAC indica si el receptor está sincronizado a una transmisión DRM o no.&lt;br&gt;El ancho de banda de la señal DRM, el esquema de constelación para los canales MSC y SDC y la profundidad del intercalado son algunos de los parámetros provistos por el FAC.</translation>
    </message>
    <message>
        <source>&lt;b&gt;SDC CRC LED:&lt;/b&gt; This LED shows the CRC check result of the Service Description Channel (SDC) which is one logical channel of the DRM stream. This data is transmitted in approx. 1 second intervals and contains information about station label, audio and data format, etc. The error protection is normally lower than the protection of the FAC. Therefore this LED will turn to red earlier than the FAC LED in general.&lt;br&gt;If the CRC check is ok but errors in the content were detected, the LED turns yellow.</source>
        <translation type='unfinished'>&lt;b&gt;LED de CRC de canal SDC:&lt;/b&gt; Este LED indica el CRC (Cyclic Redundancy Check) del canal SDC (Service Description Channel). SDC es uno de los tres canales lógicos de DRM. Estos datos se transmiten en intervalos de aproximadamente un segundo y contienen información sobre el nombre de la estación, formatos de audio y datos, etc. El nivel de protección frente a errores es normalmente más bajo que el del FAC. Por tanto, este LED se pondrá generalmente de color rojo antes que el del FAC.&lt;br&gt;Si la comprobación del CRC es correcta pero se detectan errores en el contenido, el LED se vuelve de color amarillo.</translation>
    </message>
    <message>
        <source>&lt;b&gt;MSC CRC LED:&lt;/b&gt; This LED shows the status of the Main Service Channel (MSC). This channel contains the actual audio and data bits. The LED shows the CRC check of the AAC core decoder. The SBR has a separate CRC, but this status is not shown with this LED. If SBR CRC is wrong but the AAC CRC is ok one can still hear something (of course, the high frequencies are not there in this case). If this LED turns red, interruptions of the audio are heard. The yellow light shows that only one 40 ms audio frame CRC was wrong. This causes usually no hearable artifacts.</source>
        <translation type='unfinished'>&lt;b&gt;LED de CRC de canal MSC:&lt;/b&gt; Este LED indica el CRC (Cyclic Redundancy Check) del canal MSC (Main Service Channel). Este canal contiene el audio y los bits de datos reales. El LED indica la comprobación del CRC del decodificador principal AAC. SBR (Spectral Band Replication) tiene un CRC separado, pero su estado no se muestra con este LED. Si el CRC de la parte SBR es erróneo y el de la parte AAC es correcto, se puede oír algo (aunque en este caso no se escuchan las altas frecuencias). Si este LED se vuelve de color rojo, se oyen interrupciones en el audio. La luz amarilla indica que sólo una trama de audio de 40 ms tenía un CRC erróneo. Normalmente, esto no causa efectos escuchables.</translation>
    </message>
    <message>
        <source>&lt;b&gt;MLC, Number of Iterations:&lt;/b&gt; In DRM, a multilevel channel coder is used. With this code it is possible to iterate the decoding process in the decoder to improve the decoding result. The more iterations are used the better the result will be. But switching to more iterations will increase the CPU load. Simulations showed that the first iteration (number of iterations = 1) gives the most improvement (approx. 1.5 dB at a BER of 10-4 on a Gaussian channel, Mode A, 10 kHz bandwidth). The improvement of the second iteration will be as small as 0.3 dB.&lt;br&gt;The recommended number of iterations given in the DRM standard is one iteration (number of iterations = 1).</source>
        <translation type='unfinished'>&lt;b&gt;MLC, Número de Iteraciones:&lt;/b&gt; En DRM se utiliza un codificador de canal multinivel. Con este código, es posible iterar en el proceso de decodificación para mejorar el resultado de la misma. Cuantas más iteraciones se empleen, mejores resultados se obtendrán. Pero cambiar a un mayor número de iteraciones aumentará la carga de la CPU. Mediante simulaciones se ha probado que la primera iteración (número de iteraciones = 1) proporciona la mejora más significativa (aproximadamente 1.5 dB a una BER de 10^-4 en un canal Gaussiano, Modo A, ancho de banda de 10 kHz). La mejora de la segunda iteración será como mucho de 0.3 dB.&lt;br&gt;El número de iteraciones recomendado que da el estándar de DRM es una iteración (Nº de iteraciones = 1).</translation>
    </message>
    <message>
        <source>&lt;b&gt;Flip Input Spectrum:&lt;/b&gt; Checking this box will flip or invert the input spectrum. This is necessary if the mixer in the front-end uses the lower side band.</source>
        <translation type='unfinished'>&lt;b&gt;Invertir Espectro de Entrada:&lt;/b&gt; Marcar esta opción invertirá o volteará el espectro de entrada. Esto es necesario si el mezclador del receptor utiliza la banda lateral inferior.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Mute Audio:&lt;/b&gt; The audio can be muted by checking this box. The reaction of checking or unchecking this box is delayed by approx. 1 second due to the audio buffers.</source>
        <translation type='unfinished'>&lt;b&gt;Silenciar Audio:&lt;/b&gt; Al marcar este cuadro el audio puede silenciarse. Al marcar/desmarcar este cuadro la reacción puede retardarse aproximadamente un segundo, debido a los buffers de audio.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Reverberation Effect:&lt;/b&gt; If this check box is checked, a reverberation effect is applied each time an audio drop-out occurs. With this effect it is possible to mask short drop-outs.</source>
        <translation type='unfinished'>&lt;b&gt;Efecto de resonancia:&lt;/b&gt; Al marcar esta opción, se aplica un efecto de resonancia cada vez que hay una pérdida de información de audio. Con este efecto es posible enmascarar breves pérdidas de información.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Freq:&lt;/b&gt; In this edit control, the current selected frequency on the front-end can be specified. This frequency will be written into the log file.</source>
        <translation type='unfinished'>&lt;b&gt;Frec:&lt;/b&gt; En este cuadro de control se puede especificar la frecuencia a la que está sintonizado el receptor. Esta frecuencia se escribirá en el fichero de registro.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Channel Estimation Settings:&lt;/b&gt; With these settings, the channel estimation method in time and frequency direction can be selected. The default values use the most powerful algorithms. For more detailed information about the estimation algorithms there are a lot of papers and books available.&lt;br&gt;&lt;b&gt;Wiener:&lt;/b&gt; Wiener interpolation method uses estimation of the statistics of the channel to design an optimal filter for noise reduction.</source>
        <translation type='unfinished'>&lt;b&gt;Opciones de Estimación del Canal:&lt;/b&gt; Con estas opciones, se puede elegir el método de estimación del canal tanto en tiempo como en frecuencia. Los valores por defecto usan los algoritmos más potentes. Para información más detallada sobre los algoritmos de estimación, existen multitud de artículos y libros disponibles.&lt;br&gt;&lt;b&gt;Wiener:&lt;/b&gt; El método de interpolación de Wiener usa una estimación de los estadísticos del canal para diseñar un filtro óptimo para la reducción de ruido.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Channel Estimation Settings:&lt;/b&gt; With these settings, the channel estimation method in time and frequency direction can be selected. The default values use the most powerful algorithms. For more detailed information about the estimation algorithms there are a lot of papers and books available.&lt;br&gt;&lt;b&gt;Linear:&lt;/b&gt; Simple linear interpolation method to get the channel estimate. The real and imaginary parts of the estimated channel at the pilot positions are linearly interpolated. This algorithm causes the lowest CPU load but performs much worse than the Wiener interpolation at low SNRs.</source>
        <translation type='unfinished'>&lt;b&gt;Opciones de Estimación del Canal:&lt;/b&gt; Con estas opciones, se puede elegir el método de estimación del canal tanto en tiempo como en frecuencia. Los valores por defecto usan los algoritmos más potentes. Para información más detallada sobre los algoritmos de estimación, existen multitud de artículos y libros disponibles.&lt;br&gt;&lt;b&gt;Lineal:&lt;/b&gt; Método simple de interpolación lineal para conseguir una estimación del canal. Se interpolan linealmente las partes real e imaginaria del canal estimado en las posiciones de los tonos piloto. Este algoritmo es el que implica una menor carga para la CPU pero su rendimiento es peor que el de Wiener para SNRs bajas.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Channel Estimation Settings:&lt;/b&gt; With these settings, the channel estimation method in time and frequency direction can be selected. The default values use the most powerful algorithms. For more detailed information about the estimation algorithms there are a lot of papers and books available.&lt;br&gt;&lt;b&gt;DFT Zero Pad:&lt;/b&gt; Channel estimation method for the frequency direction using Discrete Fourier Transformation (DFT) to transform the channel estimation at the pilot positions to the time domain. There, a zero padding is applied to get a higher resolution in the frequency domain -&gt; estimates at the data cells. This algorithm is very speed efficient but has problems at the edges of the OFDM spectrum due to the leakage effect.</source>
        <translation type='unfinished'>&lt;b&gt;Opciones de Estimación del Canal:&lt;/b&gt; Con estas opciones, se puede elegir el método de estimación del canal tanto en tiempo como en frecuencia. Los valores por defecto usan los algoritmos más potentes. Para información más detallada sobre los algoritmos de estimación, existen multitud de artículos y libros disponibles.&lt;br&gt;&lt;b&gt;Relleno de ceros en la DFT:&lt;/b&gt; Método de estimación del canal en el dominio de la frecuencia usando la DFT (Discrete Fourier Transform) para transformar la estimación del canal en las posiciones de los tonos pilotos al dominio del tiempo. Se aplica un relleno con ceros para conseguir una mayor resolución en el dominio de la frecuencia --&gt; estimaciones en las celdas de datos. Este algoritmo es muy eficiente en velocidad pero tiene problemas en los extremos del espectro OFDM debido al efecto de fuga ("leakage effect").</translation>
    </message>
    <message>
        <source>&lt;b&gt;Guard Energy:&lt;/b&gt; Time synchronization tracking algorithm utilizes the estimation of the impulse response. This method tries to maximize the energy in the guard-interval to set the correct timing.</source>
        <translation type='unfinished'>&lt;b&gt;Energía de Guarda:&lt;/b&gt; El algoritmo de seguimiento de sincronización temporal utiliza la estimación de la respuesta impulsional. Este método intenta maximizar la energía en el intervalo de guarda para ajustar la coordinación adecuada.</translation>
    </message>
    <message>
        <source>&lt;b&gt;First Peak:&lt;/b&gt; This algorithms searches for the first peak in the estimated impulse response and moves this peak to the beginning of the guard-interval (timing tracking algorithm).</source>
        <translation type='unfinished'>&lt;b&gt;Primer pico:&lt;/b&gt; Este algoritmo busca el primer pico en la respuesta impulsional estimada y mueve este pico al comienzo del intervalo de guarda (algoritmo de seguimiento temporal).</translation>
    </message>
    <message>
        <source>&lt;b&gt;SNR:&lt;/b&gt; Signal to Noise Ratio (SNR) estimation based on FAC cells. Since the FAC cells are only located approximately in the region 0-5 kHz relative to the DRM DC frequency, it may happen that the SNR value is very high although the DRM spectrum on the left side of the DRM DC frequency is heavily distorted or disturbed by an interferer so that the true overall SNR is lower as indicated by the SNR value. Similarly, the SNR value might show a very low value but audio can still be decoded if only the right side of the DRM spectrum is degraded by an interferer.</source>
        <translation type='unfinished'>&lt;b&gt;Relación Señal a Ruido (SNR):&lt;/b&gt; Se trata de una estimación de la SNR (Signal to Noise Ratio) basada en celdas del canal FAC. Dado que las celdas FAC están únicamente situadas aproximadamente en la región 0 - 5 kHz relativa a la frecuencia DC de la señal DRM, puede ocurrir que el valor de la SNR estimada sea muy alto aunque el espectro DRM de la parte derecha de la frecuencia DC esté gravemente distorsionado o interrumpido por una señal interferente, de modo que la SNR total real es más baja que lo indicado por el valor de la SNR. Análogamente,  puede ocurrir que el valor de la SNR muestre un valor muy bajo y el audio se pueda decodificar si es la parte derecha del espectro DRM la degradada por una señal interferente.</translation>
    </message>
    <message>
        <source>&lt;b&gt;MSC WMER / MSC MER:&lt;/b&gt; Modulation Error Ratio (MER) and weighted MER (WMER) calculated on the MSC cells is shown. The MER is calculated as follows: For each equalized MSC cell (only MSC cells, no FAC cells, no SDC cells, no pilot cells), the error vector from the nearest ideal point of the constellation diagram is measured. The squared magnitude of this error is found, and a mean of the squared errors is calculated (over one frame). The MER is the ratio in [dB] of the mean of the squared magnitudes of the ideal points of the constellation diagram to the mean squared error. This gives an estimate of the ratio of the total signal power to total noise power at the input to the equalizer for channels with flat frequency response.&lt;br&gt; In case of the WMER, the calculations of the means are multiplied by the squared magnitude of the estimated channel response.&lt;br&gt;For more information see ETSI TS 102 349.</source>
        <translation type='unfinished'>&lt;b&gt;WMER del canal MSC / MER del canal MSC:&lt;/b&gt; Se muestra la MER (Tasa de Error de Modulación) y la WMER (MER Ponderada). La MER se calcula de la siguiente manera: Por cada celda MSC ecualizada (sólo celdas MSC; no celdas FAC, ni celdas SDC, ni celdas piloto), se mide el vector de error desde el punto ideal más cercano del diagrama de la constelación. Se halla el cuadrado del módulo de este error y se calcula el promedio (sobre una trama). La MER es el ratio en [dB] entre la media de los cuadrados de los puntos ideales del diagrama de la constelación y el error cuadrático medio. Esto da una estimación del ratio entre la potencia total de la señal y la potencia total de ruido a la entrada del ecualizador para canales con respuesta en frecuencia plana.&lt;br&gt;En el caso de la WMER, los cálculos de las medias se multiplican por el módulo al cuadrado de la respuesta del canal estimada.&lt;br&gt;Para más información, ver ETSI TS 102 349 [DRM Receiver Status and Control Interface (RSCI)].</translation>
    </message>
    <message>
        <source>&lt;b&gt;DRM Mode / Bandwidth:&lt;/b&gt; In a DRM system, four possible robustness modes are defined to adapt the system to different channel conditions. According to the DRM standard:&lt;ul&gt;&lt;li&gt;&lt;i&gt;Mode A:&lt;/i&gt; Gaussian channels, with minor fading&lt;/li&gt;&lt;li&gt;&lt;i&gt;Mode B:&lt;/i&gt; Time and frequency selective channels, with longer delay spread&lt;/li&gt;&lt;li&gt;&lt;i&gt;Mode C:&lt;/i&gt; As robustness mode B, but with higher Doppler spread&lt;/li&gt;&lt;li&gt;&lt;i&gt;Mode D:&lt;/i&gt; As robustness mode B, but with severe delay and Doppler spread&lt;/li&gt;&lt;/ul&gt;The bandwith is the gross bandwidth of the current DRM signal</source>
        <translation type='unfinished'>&lt;b&gt;Modo DRM / Ancho de Banda:&lt;/b&gt; En un sistema DRM, se definen cuatro modos de robustez posibles para adaptar el sistema a las diferentes condiciones del canal. Según el estándar de DRM:&lt;ul&gt;&lt;li&gt;&lt;i&gt;Modo A:&lt;/i&gt; Canales Gaussianos, con escaso desvanecimiento&lt;/li&gt;&lt;li&gt;&lt;i&gt;Modo B:&lt;/i&gt; Canales Selectivos en Tempo y en Frecuencia, con mayor propagación de retardo&lt;/li&gt;&lt;li&gt;&lt;i&gt;Modo C:&lt;/i&gt; Como el modo de robustez B, pero con mayor propagación Doppler&lt;/li&gt;&lt;li&gt;&lt;i&gt;Modo D:&lt;/i&gt; Como el modo de robustez C, pero con retardo severo&lt;/li&gt;&lt;/ul&gt;El ancho de banda es el ancho de banda bruto de la señal DRM actual.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Interleaver Depth:&lt;/b&gt; The symbol interleaver depth can be either short (approx. 400 ms) or long (approx. 2 s). The longer the interleaver the better the channel decoder can correct errors from slow fading signals. But the longer the interleaver length the longer the delay until (after a re-synchronization) audio can be heard.</source>
        <translation type='unfinished'>&lt;b&gt;Modo de Intercalado MSC:&lt;/b&gt; La profundidad del intercalado de símbolos puede ser bien corta (aproximadamente 400 ms) o bien larga (aproximadamente 2 s). Mientras mayor sea la longitud de intercalado, mejor podrá el decodificador de canal corregir errores en señales con escaso desvanecimiento, aunque también será mayor el retardo (después de una resincronización) hasta que pueda oírse el audio.</translation>
    </message>
    <message>
        <source>&lt;b&gt;SDC / MSC Mode:&lt;/b&gt; Shows the modulation type of the SDC and MSC channel. For the MSC channel, some hierarchical modes are defined which can provide a very strong protected service channel.</source>
        <translation type='unfinished'>&lt;b&gt;Modo SDC / MSC:&lt;/b&gt; Muestra el tipo de modulación de los canales SDC y MSC. Para el canal MSC, se definen algunos modos jerárquicos que pueden proveer un canal de servicio fuertemente protegido.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Prot. Level (B/A):&lt;/b&gt; The error protection level of the channel coder. For 64-QAM, there are four protection levels defined in the DRM standard. Protection level 0 has the highest protection whereas level 3 has the lowest protection. The letters A and B are the names of the higher and lower protected parts of a DRM block when Unequal Error Protection (UEP) is used. If Equal Error Protection (EEP) is used, only the protection level of part B is valid.</source>
        <translation type='unfinished'>&lt;b&gt;Nivel de Protección (B/A):&lt;/b&gt; Es el nivel de protección de error del codificador de canal. Para una 64-QAM se definen cuatro niveles de protección en el estándar de DRM. En nivel de protección 0 tienen la máxima protección, mientras que el nivel de protección 3 tiene la mínima. Las letras A y B son los nombres de las partes más y menos protegidas de un bloque DRM cuando se emplea UEP (Unequal Error Protection). Si se usa EEP (Equal Error Protection), sólo el nivel de protección de parte B es válido.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Number of Services:&lt;/b&gt; This shows the number of audio and data services transmitted in the DRM stream. The maximum number of streams is four.</source>
        <translation type='unfinished'>&lt;b&gt;Número de Servicios:&lt;/b&gt; Muestra el número de servicios de audio y de datos transmitidos en el flujo DRM. El máximo número de tramas es cuatro.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Received time - date:&lt;/b&gt; This label shows the received time and date in UTC. This information is carried in the SDC channel.</source>
        <translation type='unfinished'>&lt;b&gt;Hora / Fecha Recibidas:&lt;/b&gt; Esta etiqueta muestra la hora y fecha recibidas en UTC. Esta información se transmite en el canal SDC (Service Description Channel).</translation>
    </message>
    <message>
        <source>&lt;b&gt;Save Audio as WAV:&lt;/b&gt; Save the audio signal as stereo, 16-bit, 48 kHz sample rate PCM wave file. Checking this box will let the user choose a file name for the recording.</source>
        <translation type='unfinished'>&lt;b&gt;Guardar Audio como WAV:&lt;/b&gt; Guarda la señal de audio como un archivo de onda PCM (Pulse Code Modulation) estéreo de 16 bits y con una tasa de muestreo de 48 kHz. Al marcar este cuadro, el usuario podrá elegir un nombre de archivo para la grabación.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Chart Selector:&lt;/b&gt; With the chart selector different types of graphical display of parameters and receiver states can be chosen. The different plot types are sorted in different groups. To open a group just double-click on the group or click on the plus left of the group name. After clicking on an item it is possible to choose other items by using the up / down arrow keys. With these keys it is also possible to open and close the groups by using the left / right arrow keys.&lt;br&gt;A separate chart window for a selected item can be opened by right click on the item and click on the context menu item.</source>
        <translation type='unfinished'>&lt;b&gt;Selector de Gráfico:&lt;/b&gt; Mediante esta opción se pueden seleccionar diferentes tipos de visualización gráfica de parámetros y de estados del receptor. Los diferentes tipos de representación están agrupados en diversos grupos. Para abrir un grupo simplemente ha de hacer doble click sobre el grupo o hacer click en el símbolo &apos;más&apos; a la izquierda del nombre del grupo. Después de clickear sobre un objeto es posible escoger otros usando las flechas arriba / abajo. Con estas teclas también es posible abrir y cerrar los grupos usando las teclas de flecha izquierda / derecha.&lt;br&gt;Se puede abrir una nueva ventana gráfica para un elemento determinado haciendo click con el botón derecho sobre el elemento y clikeando en el menú contextual del objeto.</translation>
    </message>
    <message>
        <source>&lt;b&gt;Interferer Rejection:&lt;/b&gt; There are two algorithms available to reject interferers:&lt;ul&gt;&lt;li&gt;&lt;b&gt;Bandpass Filter (BP-Filter):&lt;/b&gt; The bandpass filter is designed to have the same bandwidth as the DRM signal. If, e.g., a strong signal is close to the border of the actual DRM signal, under some conditions this signal will produce interference in the useful bandwidth of the DRM signal although it is not on the same frequency as the DRM signal. The reason for that behaviour lies in the way the OFDM demodulation is done. Since OFDM demodulation is a block-wise operation, a windowing has to be applied (which is rectangular in case of OFDM). As a result, the spectrum of a signal is convoluted with a Sinc function in the frequency domain. If a sinusoidal signal close to the border of the DRM signal is considered, its spectrum will not be a distinct peak but a shifted Sinc function. So its spectrum is broadened caused by the windowing. Thus, it will spread in the DRM spectrum and act as an in-band interferer.&lt;br&gt;There is a special case if the sinusoidal signal is in a distance of a multiple of the carrier spacing of the DRM signal. Since the Sinc function has zeros at certain positions it happens that in this case the zeros are exactly at the sub-carrier frequencies of the DRM signal. In this case, no interference takes place. If the sinusoidal signal is in a distance of a multiple of the carrier spacing plus half of the carrier spacing away from the DRM signal, the interference reaches its maximum.&lt;br&gt;As a result, if only one DRM signal is present in the 20 kHz bandwidth, bandpass filtering has no effect. Also,  if the interferer is far away from the DRM signal, filtering will not give much improvement since the squared magnitude of the spectrum of the Sinc function is approx -15 dB down at 1 1/2 carrier spacing (approx 70 Hz with DRM mode B) and goes down to approx -30 dB at 10 times the carrier spacing plus 1 / 2 of the carrier spacing (approx 525 Hz with DRM mode B). The bandpass filter must have very sharp edges otherwise the gain in performance will be very small.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Modified Metrics:&lt;/b&gt; Based on the information from the SNR versus sub-carrier estimation, the metrics for the Viterbi decoder can be modified so that sub-carriers with high noise are attenuated and do not contribute too much to the decoding result. That can improve reception under bad conditions but may worsen the reception in situations where a lot of fading happens and no interferer are present since the SNR estimation may be not correct.&lt;/li&gt;&lt;/ul&gt;</source>
        <translation type='unfinished'>&lt;b&gt;Rechazo a Interferencias:&lt;/b&gt; Hay dos algoritmos disponibles para el rechazo contra interferentes:&lt;ul&gt;&lt;li&gt;&lt;b&gt;Filtro Paso Banda (BP-Filter):&lt;/b&gt; El filtro paso banda se diseña para que tenga el mismo ancho de banda que la señal DRM. Si, por ejemplo, una señal fuerte está próxima al borde de la señal DRM real, bajo algunas condiciones esta señal producirá interferencia en el ancho de banda útil de la señal DRM aunque no esté en la misma frecuencia que la señal DRM. La razón de ese comportamiento reside en la forma de realizar la demodulación OFDM. Dado que la demodulación OFDM es una operación de bloques, se ha de aplicar un enventanado (que en el caso de OFDM es rectangular). Como resultado, el espectro de la señal se convoluciona con una función Sinc en el dominio de la frecuencia. Si se considerara como señal próxima al borde de la señal DRM una señal sinusoidal, su espectro no sería un pico marcado sino una función Sinc desplazada. Así que su espectro se ensancha debido al enventanado. De este modo, se ensanchará dentro del espectro DRM y actuará como una señal interferente en su propia banda (in-band).&lt;br&gt;Hay un caso especial si la señal sinuosoidal está a una distancia múltiplo del espaciado de portadora de la señal DRM. Dado que la función Sinc tiene ceros en ciertas posiciones, ocurre que en este caso los ceros coinciden con las frecuencias de subportadoras de la señal DRM. Por tanto, no hay interferencia para este caso concreto. En cambio, si la señal sinusoidal está a una distancia múltiplo del espaciado de portadora más la mitad de éste de la señal DRM, el nivel de interferencia alcanza su máximo valor.&lt;br&gt;Como resultado, si la señal DRM es la única presente en el ancho de banda de los 20 kHz, el filtrado paso banda no tiene efecto alguno. Igualmente, si la señal interferente está lejos de la señal DRM, el filtrado no aportará mucha mejoría ya que el cuadrado del módulo del espectro de la función Sinc se encuentra aproximadamente 15 dB por debajo a una distancia de media portadora (aproximadamente 70 Hz en modo B) y 30 dB por debajo a una distancia de diez veces y media la distancia entre portadoras (aproximadamente 525 Hz en modo B). El filtro paso banda debe ser muy abrupto en los extremos, ya que en caso contrario la ganancia en el rendimiento sería muy pequeña.&lt;/li&gt;&lt;li&gt;&lt;b&gt;Medidas Modificadas:&lt;b&gt; Basadas en la información de la SNR versus la estimación de subportadoras, los parámetros para el decodificador de Viterbi se pueden modificar de modo que las subportadoras con mucho ruido se atenúen y no contribuyan demasiado al resultado de la decodificación. Eso podría mejorar la recepción bajo malas condiciones, aunque podría empeorarla en situaciones en las que haya mucha atenuación y no haya interferencia, ya que la estimación de la SNR podría no ser correcta.&lt;/li&gt;&lt;/ul&gt;</translation>
    </message>
</context>
<context>
    <name>systemevalDlgBase</name>
    <message>
        <source>System Evaluation</source>
        <translation type='unfinished'>Evaluación del Sistema</translation>
    </message>
    <message>
        <source>SNR:</source>
        <translation type='unfinished'>SNR:</translation>
    </message>
    <message>
        <source>MSC WMER / MSC MER:</source>
        <translation type='unfinished'>WMER / MER en canal MSC:</translation>
    </message>
    <message>
        <source>DC Frequency of DRM Signal:</source>
        <translation type='unfinished'>Frecuencia DC de Señal DRM:</translation>
    </message>
    <message>
        <source>Sample Frequency Offset:</source>
        <translation type='unfinished'>Offset de Frecuencia de Muestreo:</translation>
    </message>
    <message>
        <source>Doppler / Delay:</source>
        <translation type='unfinished'>Doppler / Retardo:</translation>
    </message>
    <message>
        <source>SNR</source>
        <translation type='unfinished'>SNR</translation>
    </message>
    <message>
        <source>MER</source>
        <translation type='unfinished'>MER</translation>
    </message>
    <message>
        <source>FreqOffset</source>
        <translation type='unfinished'>OffsetFrec</translation>
    </message>
    <message>
        <source>SampFreqOffset</source>
        <translation type='unfinished'>OffsetFrecMuestreo</translation>
    </message>
    <message>
        <source>Wiener</source>
        <translation type='unfinished'>Wiener</translation>
    </message>
    <message>
        <source>MSC CRC:</source>
        <translation type='unfinished'>CRC de MSC:</translation>
    </message>
    <message>
        <source>SDC CRC:</source>
        <translation type='unfinished'>CRC de MSC:</translation>
    </message>
    <message>
        <source>FAC CRC:</source>
        <translation type='unfinished'>CRC  de FAC:</translation>
    </message>
    <message>
        <source>Frame Sync:</source>
        <translation type='unfinished'>Sinc. de Trama:</translation>
    </message>
    <message>
        <source>Time Sync Acq:</source>
        <translation type='unfinished'>Adq. Sinc. Temporal:</translation>
    </message>
    <message>
        <source>I/O Interface:</source>
        <translation type='unfinished'>Interfaz E/S:</translation>
    </message>
    <message>
        <source>FACDRMModeBWL</source>
        <translation type='unfinished'>FACDRMModoBWL</translation>
    </message>
    <message>
        <source>FACInterleaverDepthL</source>
        <translation type='unfinished'>ProfundidadIntercaladoFACL</translation>
    </message>
    <message>
        <source>FACSDCMSCModeL</source>
        <translation type='unfinished'>FACSDCMSCModoL</translation>
    </message>
    <message>
        <source>FACCodeRateL</source>
        <translation type='unfinished'>TasaCodigoFACL</translation>
    </message>
    <message>
        <source>FACNumServicesL</source>
        <translation type='unfinished'>NumServiciosFACL</translation>
    </message>
    <message>
        <source>FACTimeDateL</source>
        <translation type='unfinished'>HoraFechaFACL</translation>
    </message>
    <message>
        <source>FACDRMModeBWV</source>
        <translation type='unfinished'>FACDRMModoBWV</translation>
    </message>
    <message>
        <source>FACInterleaverDepthV</source>
        <translation type='unfinished'>ProfundidadIntercaladoFACV</translation>
    </message>
    <message>
        <source>FACSDCMSCModeV</source>
        <translation type='unfinished'>FACSDCMSCModoV</translation>
    </message>
    <message>
        <source>FACCodeRateV</source>
        <translation type='unfinished'>TasaCodigoFACV</translation>
    </message>
    <message>
        <source>FACNumServicesV</source>
        <translation type='unfinished'>NumServiciosFACV</translation>
    </message>
    <message>
        <source>FACTimeDateV</source>
        <translation type='unfinished'>HoraFechaFACV</translation>
    </message>
    <message>
        <source>Chart Selector</source>
        <translation type='unfinished'>Selector de Gráfico</translation>
    </message>
    <message>
        <source>D-Spectrum</source>
        <translation type='unfinished'>Espectro-D</translation>
    </message>
    <message>
        <source>D-InpPSD</source>
        <translation type='unfinished'>DEPEntrada-D</translation>
    </message>
    <message>
        <source>D-InpSpectr</source>
        <translation type='unfinished'>EspectroEnt-D</translation>
    </message>
    <message>
        <source>D-Waterfall</source>
        <translation type='unfinished'>Cascada-D</translation>
    </message>
    <message>
        <source>D-ShiftedPSD</source>
        <translation type='unfinished'>DEPDespl-D</translation>
    </message>
    <message>
        <source>D-AudSpectr</source>
        <translation type='unfinished'>EspectroAudio-D</translation>
    </message>
    <message>
        <source>D-SNRSpectr</source>
        <translation type='unfinished'>EspectroSNR-D</translation>
    </message>
    <message>
        <source>D-Channel</source>
        <translation type='unfinished'>Canal-D</translation>
    </message>
    <message>
        <source>D-IR</source>
        <translation type='unfinished'>IR-D</translation>
    </message>
    <message>
        <source>D-TF</source>
        <translation type='unfinished'>TF-D</translation>
    </message>
    <message>
        <source>D-Constellation</source>
        <translation type='unfinished'>Constelación-D</translation>
    </message>
    <message>
        <source>D-FAC</source>
        <translation type='unfinished'>FAC-D</translation>
    </message>
    <message>
        <source>D-SDC</source>
        <translation type='unfinished'>SDC-D</translation>
    </message>
    <message>
        <source>D-MSC</source>
        <translation type='unfinished'>MSC-D</translation>
    </message>
    <message>
        <source>D-History</source>
        <translation type='unfinished'>Historial-D</translation>
    </message>
    <message>
        <source>D-SNRAudiHist</source>
        <translation type='unfinished'>SNRAudioHIst-D</translation>
    </message>
    <message>
        <source>D-DelDopp</source>
        <translation type='unfinished'>RetardoDoppler-D</translation>
    </message>
    <message>
        <source>D-FreqSam</source>
        <translation type='unfinished'>TasaMuestr-D</translation>
    </message>
    <message>
        <source>Channel Estimation: Frequency Interpolation</source>
        <translation type='unfinished'>Estimación de Canal: Interpolación en Dom. Frecuencia</translation>
    </message>
    <message>
        <source>Linear</source>
        <translation type='unfinished'>Lineal</translation>
    </message>
    <message>
        <source>DFT Zero Pad.</source>
        <translation type='unfinished'>Relleno de ceros en DFT</translation>
    </message>
    <message>
        <source>Channel Estimation: Time Interpolation</source>
        <translation type='unfinished'>Estimación de Canal: Interpolación en Dom. Tiempo</translation>
    </message>
    <message>
        <source>Time Sync Tracking</source>
        <translation type='unfinished'>Seguimiento de Sincronización Temporal</translation>
    </message>
    <message>
        <source>Guard Energy</source>
        <translation type='unfinished'>Energía de Guarda</translation>
    </message>
    <message>
        <source>First Peak</source>
        <translation type='unfinished'>Primer Pico</translation>
    </message>
    <message>
        <source>Misc Settings</source>
        <translation type='unfinished'>Opciones Varias</translation>
    </message>
    <message>
        <source>TextNumOfIterations</source>
        <translation type='unfinished'>TextoNumeroIteraciones</translation>
    </message>
    <message>
        <source>Log File, Audio</source>
        <translation type='unfinished'>Archivo de Registro, Audio</translation>
    </message>
    <message>
        <source>kHz</source>
        <translation type='unfinished'>kHz</translation>
    </message>
    <message>
        <source>Interferer Rejection</source>
        <translation type='unfinished'>Rechazo a Interferencias</translation>
    </message>
    <message>
        <source>&amp;Close</source>
        <translation type='unfinished'>&amp;Cerrar</translation>
    </message>
    <message>
        <source>&amp;Flip Input Spectrum</source>
        <translation type='unfinished'>&amp;Voltear Espectro de Entrada</translation>
    </message>
    <message>
        <source>Mute &amp;Audio</source>
        <translation type='unfinished'>Silenciar &amp;Audio</translation>
    </message>
    <message>
        <source>&amp;Log File, Freq:</source>
        <translation type='unfinished'>&amp;Archivo de Registro, Frec:</translation>
    </message>
    <message>
        <source>&amp;Save Audio as WAV</source>
        <translation type='unfinished'>&amp;Guardar Audio como WAV</translation>
    </message>
    <message>
        <source>&amp;Reverberation</source>
        <translation type='unfinished'>&amp;Resonancia</translation>
    </message>
    <message>
        <source>&amp;Bandpass Filter</source>
        <translation type='unfinished'>Filtro Paso &amp;Banda</translation>
    </message>
    <message>
        <source>Modified &amp;Metrics</source>
        <translation type='unfinished'>Medidas &amp;Modificadas</translation>
    </message>
</context>
</TS>
