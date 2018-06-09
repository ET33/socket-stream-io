      ______     ______     __         ______     __    __     ______     __   __    
     /\  ___\   /\  __ \   /\ \       /\  __ \   /\ "-./  \   /\  __ \   /\ "-.\ \   
     \ \___  \  \ \ \/\ \  \ \ \____  \ \ \/\ \  \ \ \-./\ \  \ \ \/\ \  \ \ \-.  \  
      \/\_____\  \ \_____\  \ \_____\  \ \_____\  \ \_\ \ \_\  \ \_____\  \ \_\\"\_\ 
       \/_____/   \/_____/   \/_____/   \/_____/   \/_/  \/_/   \/_____/   \/_/ \/_/ 

     ------------------------------------------------------------------------------

# A Socket Stream Application
Solomon nada mais é do que uma aplicação de sockets em C que faz streaming de áudio entre um cliente e um servidor.

## Minimum Viable Product (MVP)
Para o MVP algumas funcionalidaes devem ser implementadas, dentre elas temos:

No lado do cliente:
- Se conectar a um servidor;
- Listar músicas disponíveis no servidor com um determinado número na frente para facilitar escolha;
     - Exemplo: [1] Toto - Africa;
- Dar play em alguma música;
- Dar stop no player;
- Tocar as músicas e não travar a interface de comunicação com o servidor;
- Fechar o player e sair da aplicação.

No lado do servidor:
- Aceitar conexão de um cliente;
- Prover interface de comandos para o cliente;
- Permitir escolher o caminho das músicas;
- Enviar as músicas para o cliente em forma de streaming.

## Expansion Pack
Algumas funcionalidades a mais que podem ser implementadas caso tenhamos tempo:

No lado do cliente:
- Dar pause na música;
- Dar next na música;
- Dar previous na música;
- Shuffle;
- Repeat once;
- Possibilidade de reconexão ao servidor após encerramento do cliente.

No lado do servidor:
- Prover interface que aceite os novos comandos do cliente;
- Permitir múltiplas conexões de clientes;
- Listar número de clientes conectados;
- Manter o servidor sempre ativo, independente de ter cliente ou não;
     - O administrador do servidor daria o comando EXIT para encerrar o servidor;
     - Um limite máximo de conexões pode ser estabelecido, exemplo: 10 conexões;
     - O servidor teria uma lista de conexões ativas e disponíveis;
          - A cada nova conexão o contador seria incrementado até chegar ao limite;
          - Caso um cliente se desconecte, o servidor liberaria a conexão para outro cliente.
