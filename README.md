# Sincronização do Metrô (Implementação 4)

Disciplina de Infraestrutura de Software, CESAR School. Login: gsg4.

Implementação das funções de sincronização entre carros e passageiros de uma estação, usando threads POSIX (mutex e variáveis de condição).

## Sistema operacional

Desenvolvido e testado em Ubuntu 24.04 (WSL2 no Windows 11), com gcc 13.3.0 e GNU Make 4.3.

## Arquivos

| Arquivo | Descrição |
|---|---|
| `sync_car.h` | Declara `struct station` e os protótipos de `station_init`, `station_load_car`, `station_wait_for_car` e `station_on_board`. |
| `sync_car.c` | Implementação das quatro funções. É o arquivo avaliado. |
| `sync_car-runner.c` | Programa de teste fornecido pela professora. Contém o `main`. Não foi modificado. |
| `Makefile` | Compila e gera o executável `sync_car`. Tem a regra `clean`. |

## Como compilar

```bash
make
```

Gera o executável `sync_car` (junta `sync_car.c` e `sync_car-runner.c`). Compila com `-Wall -Wextra -std=c11` sem warnings no código desta implementação.

## Como executar e testar

```bash
./sync_car
```

Saída esperada: várias linhas `car entering station ...` e `car departed station ...`, terminando em `Looks good!` com código de saída 0.

Para repetir o teste várias vezes (a quantidade de assentos de cada carro é sorteada, então cada execução é diferente):

```bash
for i in $(seq 1 300); do ./sync_car > /dev/null || echo FALHA; done
```

## Como limpar

```bash
make clean
```

## Funcionamento resumido

A `struct station` tem um único mutex, duas variáveis de condição e três contadores:

- `waiting`: passageiros esperando na estação.
- `to_board`: vagas que o carro ainda liberou e nenhum passageiro ocupou.
- `boarding`: passageiros que ocuparam uma vaga e ainda não chamaram `station_on_board`.

Fluxo:

1. `station_wait_for_car` conta o passageiro em `waiting` e dorme em `car_arrived` enquanto `to_board` for zero. Ao acordar, ocupa uma vaga.
2. `station_load_car` retorna imediatamente se `count` for zero ou se não houver ninguém esperando. Caso contrário libera `min(waiting, count)` vagas, acorda os passageiros e dorme em `all_boarded` até todas as vagas serem ocupadas e todos os passageiros terem chamado `station_on_board`.
3. `station_on_board` decrementa `boarding` e avisa o carro quando não há mais vagas nem passageiros embarcando.

Toda espera usa `while` com `pthread_cond_wait`, sem espera ocupada.

## Restrições do enunciado atendidas

- Apenas `pthread_mutex_init`, `pthread_mutex_lock`, `pthread_mutex_unlock`, `pthread_cond_init`, `pthread_cond_signal`, `pthread_cond_wait` e `pthread_cond_broadcast`.
- Sem semáforos.
- Um único lock por `struct station`.
- Sem espera ocupada.

## Premissas

- O carro leva exatamente `min(passageiros esperando, assentos livres)`.
- Só embarcam os passageiros que já estavam esperando quando o carro chegou.
- Com `count == 0`, ou sem passageiros esperando, `station_load_car` retorna na hora.
- Nunca há mais de um carro na estação ao mesmo tempo.
