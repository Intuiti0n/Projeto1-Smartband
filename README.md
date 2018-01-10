# Projeto 1 - SmartBand

Elementos do grupo:<br>
Daniel Gomes A74729<br>
Pedro Vila-Chã A76468<br>
Tiago Ferreira A71970<br>


<h1>OBJETIVOS:</h1> 
<ul>
  <li>Conceção, implementação e teste do sensor de batimento cardíaco. </li>
  <li>Implementação em PCB</li>
  <li>Visualização de dados em tempo real, pulsação vista no pc, BPM por bluetooth</li>
  <li>teste e possível implementação no projeto de filtros digitais</li>
  <li>implementação dos circuitos numa bracelete</li>
  <li>teste e possível implementação no projeto de filtros digitais</li>
  <li>conceção, implementação e teste do acelerómetro (passos), visualização dos dados no pc</li>
  <li> implementação de um ecrã oled</li>
  <li>visualização dos dados num dispositivo móvel (Bluetooth)
</ul>

<h1>OBJETIVOS CUMPRIDOS:</h1> 
<ul>
  <li>Conceção, implementação e teste do sensor de batimento cardíaco.  CHECK</li>
  <li>Implementação em PCB FALTA SOLDAR</li>
  <li>Visualização de dados em tempo real, pulsação vista no pc, BPM por bluetooth CHECK </li>
  <li>teste e possível implementação no projeto de filtros digitais. CHECK</li>
  <li>implementação dos circuitos numa bracelete CHECK</li>
  <li>teste e possível implementação no projeto de filtros digitais marcha. CHECK</li>
  <li>conceção, implementação e teste do acelerómetro (passos), visualização dos dados no pc. check</li>
  <li> implementação de um ecrã oled. check</li>
  <li>visualização dos dados num dispositivo móvel (Bluetooth) check </li>
</ul>

<h2>Material utilizado:</h2>
  <h3>Para o sensor de pulsação:</h3>
  <figure>
  <img src="/images/circuit.png" alt="circuit_heart" height="400">
  <figcaption>Fig1. - Implementação do sensor de pulsaçao.</figcaption>
</figure>
  <br>
  <h3>Para o sensor de passos:</h3>
  <figure>
  <img src="/images/sku_154602_2.jpg" alt="mpu-6050" height="400">
  <figcaption>Fig2. - Implementação do sensor de passos.</figcaption>
</figure>
  <br>
  
  <h3>Conceção, implementação e teste do sensor de batimento cardíaco. </h3>
  FILTRO IMPLEMENTADO EM HARDWARE<br>
  <p>Dado que a frequencia cardiaca se encontra entre os 0,83Hz(50 bpm) e os 5Hz(300bpm), dimensionamos o filtro em harware de forma a filtrar as frequencias fora desta gama.</p>
  Fc= 1/(2*pi*R*C)
  
  Fc1=0.5Hz(passa alto)
  Fc2=10Hz(passa baixo)
  
  
   <figure>
  <img src="/images/prints_relatorio/ldr_signal.png" alt="ldr_signal">
  <figcaption>Sinal do LDR</figcaption>
</figure>

<figure>
  <img src="/images/prints_relatorio/passa_alto.png" alt="passa_alto">
  <figcaption>passa_alto</figcaption>
</figure>

<figure>
  <img src="/images/prints_relatorio/passa_alto_mais_1_passa_baixo.png" alt="passa_alto_mais_1_passa_baixo">
  <figcaption>passa_alto_mais_1_passa_baixo</figcaption>
</figure>

<figure>
  <img src="/images/prints_relatorio/passa_alto_mais_2_passa_baixo.png" alt="passa_alto_mais_2_passa_baixo">
  <figcaption>passa_alto_mais_2_passa_baixo</figcaption>
</figure>


<figure>
  <img src="/images/prints_relatorio/sinal_output_amplificado.png" alt="sinal_output_amplificado">
  <figcaption>sinal_output_amplificado</figcaption>
</figure>

<figure>
  <img src="/images/prints_relatorio/comparar_2_sinais_entrada_saida.png" alt="comparar_2_sinais_entrada_saida">
  <figcaption>comparar_2_sinais_entrada_saida</figcaption>
</figure>

<h4>Diagramas de Bode dos filtros:</h4>
<figure>
  <img src="/images/prints_relatorio/Passa%20alto.PNG" alt="Passa alto">
  <figcaption>Passa alto</figcaption>
</figure>

<figure>
  <img src="/images/prints_relatorio/Passa%20baixo%201.PNG" alt="Passa baixo">
  <figcaption>Passa baixo primeiro</figcaption>
</figure>

<figure>
  <img src="/images/prints_relatorio/Passa%20baixo%202.PNG" alt="Passa baixo 2">
  <figcaption>Passa baixo segundo</figcaption>
</figure>

<figure>
  <img src="/images/prints_relatorio/Passa%20banda.PNG" alt="Passa banda">
  <figcaption>Passa banda</figcaption>
</figure>


<h4>Sinal adquirido pela porta serie</h4>
<figure>
  <img src="/images/prints_relatorio/porta_serie.png" alt="porta_serie">
  <figcaption>GRAFICO DA PORTA SERIE</figcaption>
</figure>
