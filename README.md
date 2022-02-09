# Descrição dos Arquivos
Os arquivos descritos nesse texto possuem as entradas para a realização
dos estudos numéricos (simulações) que avaliarão o sistema em estudo.
Existem dois tipos de arquivos, identificados pelas extensões .in e .original.
Os arquivos com a extensão .original são as entradas do sistema com 0(zero)
PDVs novos. O uso desse arquivo no seu sistema vai servir para que você tenha
um resultado base (baseline) do desempenho do sistema antes da inclusão de
Novos PDVs. Dessa forma você vai poder fazer o estudo comparativo do antes e
depois da inclusão dos novos PDVs na forma como o sistema se comportou com a
massa de dados(volume de clientes) que foi atendido.


Cada arquivo possui duas seções: i) *setup do sistema* e ii) *lista de eventos*.
### Setup do sistema:
1. PDV_instalado fatores_de_agilidade (igual ao nro de PDVs)
   - exemplo: *3 2 1 4*
2. Novos_PDVs fatores_de_agilidade (igual ao número de PDVs)
   - exemplo: *4 9 5 8 6*
3. Medida_de_agilidade (milisegundos)
   - exemplo: *100*
4. Tempos limites dos clientes no sistema (X, Y, Z)
   - exemplo: *8 4 2*

### Lista de Eventos:
1. Evento **chegada de clientes**
   - Tipo Tempo qtdeItens tipoCliente tempoPagamento(ms)
   - exemplo: *C 28800.483711 19 2 9*
2. Evento **suspensão de atendimento**
   - Tipo tempo PDV duração_da_suspensão(minutos)
   - exemplo: S 28803.385978 1 15
4. Evento **fim dos eventos**
   - Tipo
   - exemplo: *F*

Cada arquivo *.in* tem um arquivo correspondente *.original*. Observe que a única diferença é o número de novos PDVs na seção Setup do sistema.
