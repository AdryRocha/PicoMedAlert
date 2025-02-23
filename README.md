Sistemas Embarcados - Embarcatech - Raspberry Pi Pico W com BitDogLab.

Este repositório contém o projeto final do curso, onde exploramos o uso do microcontrolador Raspberry Pi Pico W junto à placa BitDogLab. A linguagem utilizada foi C, com simulações realizadas na plataforma Wokwi.

# 1. Projeto PicoMedAlert
Desenvolvimento de um Sistema para Monitoramento de Medicamentos com Detecção de Abertura de Tampa, utilizando a Placa BitDogLab V6.
_____________________________________________________________________________________________________
2. Descrição
Este projeto descreve um sistema responsável por monitorar o consumo de medicamentos e por emitir notificações ao usuário, o qual foi desenvolvido como entrega final do curso EmbarcaTech. O software foi elaborado na linguagem C e embarcado em Raspberry Pi Pico W, uma placa de microcontrolador de baixo custo com suporte a Wi-Fi. Devido à sua conectividade com internet (Wi-Fi e Bluetooth) e com o sensores, o sistema proposto pode ser classificado como um projeto de IoT (Internet of Things).
A estrutura deste relatório segue as diretrizes propostas por Cugnasca [13]. Com base nisto, este documento apresentará os componentes, os benefícios, o baixo custo e a viabilidade do projeto, além de fornecer uma análise acadêmica baseada em pesquisas e fontes confiáveis.
___________________________________________________________________________________________________
3. Benefícios
- Baixo Custo: Cerca de US$ 8,00 a US$ 10,00 por unidade.
- Acessibilidade: Ideal para idosos e pacientes crônicos.
- Inovação: Integração Wi-Fi e Bluethoo para monitoramento remoto.
___________________________________________________________________________________________________
4. Tecnologias Utilizadas
- Placa BitDogLab: Microcontrolador utilizado para integrar todos os componentes do projeto.
- Componentes Específicos:
2 Botões (vermelho A e azul B, pino 5 e 6)
2 LEDs (vermelho e azul, pino 12 e 11)
Resistores de 220Ω para LEDs
Tecnologias de Comunicação:
Wi-Fi
Bluethoo
Protocolo HTTP
Linguagem de Programação: C
___________________________________________________________________________________________
5. Como Instalar e Configurar o Projeto
Clone o Repositório: `git clone https://github.com/AdryRocha/PicoMedAlert.git` cd seu-repositorio

Instale as Dependências:
Instale o SDK do Pico (https://www.raspberrypi.org/documentation/).

Configure o Wi-Fi no `src/main.c` com sua rede e servidor.

Compile e Carregue o Código:

Compile o projeto utilizando um ambiente configurado com GCC e CMake para ARM

Conecte o Pico W via USB, segure BOOTSEL, e arraste o `.uf2` gerado.
______________________________________________________________________________________________________
6. Diagrama do Circuito


Contribuições
Bem-vindo a contribuições! Crie um fork, faça alterações, e envie pull requests.

Licença: Este projeto está licenciado sob a Licença MIT.  MIT License - Veja https://choosealicense.com/licenses/mit/  para detalhes.

Contato
- Nome: Adriana Rocha
- GitHub: @AdryRocha
