# Relatorio do 2° trabalho de Redes


# Introdução:

Com a assenção da computação em núvem, é cada vez mais comum que usuários de Sistemas Distribuidos confiem em sistemas remotos para o armazenamento de seus arquivos, tanto por conveniência quando por maior segurança em caso de danos ao aparelho pessoal. Nesse contexto, através desse relatório discutiremos a implementação de uma aplicação similar ao Dropbox, Mega e etc. Será apresentado os detalhes de implementação de um cliente e servidor onde é função do cliente enviar arquivos para serem aramazenados no servidor de forma eficiênte, confiável e que garanta que o arquivo seja entregue em sua totalidade.  

# Arquitetura do Cliente e do Servidor:

Para se comunicar com o servidor que irá receber os dados, o cliente utiliza o protocolo de comunicação TCP, protocolo esse que garante que a mensagem será entregue ao destinatario, e garante com alta taxa de probabilidade que o pacote será entregue com zero perdas. Esse protocolo é então usado para transmitir informações que incluem por exemplo o nome do arquivo e seu tamanho. 

Para garantir uma entrega confiável e eficiente dos dados, os dados relacionados aos arquivos foram enviados utilizando comunicação UDP, um protocolo focado em performance mas que tem como ponto negativo a não garantia da entrega dos dados ao destino solicitado e nem sua ordem. Dessa forma, um protocolo deve ser desenvolvido de forma a garantir que todos os pacotes cheguem, sendo assim utilizado e implementado um protocolo de "Janela Deslizante", protocolo esse que garante que todos os dados, dentro do arquivo serão entregues e escritos em ordem.

Sendo assim, podemos dividir a aplicação entre dois canais, o primeiro canal que deve ser aberto, o canal de controle, que é exatamente o canal que utilizará o protocolo TCP, e o segundo canal aberto, o canal de dados, que utilizará o procolo UDP.


Dessa forma, podemos definir que:
1. O canal de Controle deve escutar em uma unica porta para garantir que qualquer cliente possa enviar arquivos.  
2. Um canal de dados não pode ser compartilhado por multiplos clientes para garantir que dois clientes não corrompam os arquivos um dos outros.

Sendo assim, a sequência de comunicação do sistema fica:

1. Cliente se identifica ao servidor, enviando mensagem Hello(1).
2. Servidor responde com a porta em que o canal de dados está aberto, enviando uma mensagem de Connection(2).
3. O Cliente response à mensagem do servidor enviando ao servidor as informações do arquivo(3).
4. Servidor responde à mensagem do cliente com uma mensagem OK(4), indicando que o canal de dados está pronto para receber os dados.
5. Agora o Cliente começa a utilizar o canal de dados enviando pacotes para o servidor(5) enquanto o Servidor começa a esperar pelos dados do Cliente, e quando algum pacote chegar, o sevidor responde com uma mensagem de Acknolgment(6) para o cliente.
6. Após que todos os dados tenham sido entregues. O Servidor envia uma mensagem para o Cliente de Fim(7). A partir disso, todo o arquivo foi enviado e salvo no servidor e os Canais de Controle e de Dados são fechados.   

# Janela Deslizante:

Durante o passo 5 na sequência de troca de mensagens entre o cliente e o servidor, ocorre o envio dos dados propriamente dita, e como já dito, o protocolo UDP não é confiável, e dessa forma, deve ser implementada alguma garantia na entrega dos dados do arquivo. Sendo assim, foi adotado o protocolo de Janela Deslizante, foi implementada a janela de tipo "Selective Repeat ARQ" onde somente frames ainda não confirmados pelo servidor são re-enviados pelo cliente, esse protocolo, em pseudo código funciona da seguinte forma:

Pseudo-código para janela deslizante no Servidor:

    buffer = memoria = Lista de Lista de Bytes
    cabeça = 0
    recebidos = ListaDeBooleanos(Falso)
    Enquanto(ArquivoNãoTotalmenteRecebido()):
        pacote, posição = EsperePorUmPacote()
        SalvePacoteNoBufferNaPosicao(pacote, buffer, posição)
        recebidos[posição] = Verdadeiro
        AviseAoClienteDoRecebimentoNaPosição(posição)
        Enquanto(Igual(recebidos[cabeça],Verdadeiro)):
            SalveNoArquivo(memoria[cabeça])
            cabeça = cabeça + 1
    AviseClienteQueArquivoFoiTotalmenteRecebido()

Pseudo-código para janela deslizante no Cliente:

    enviado = ListaDeBooleanos(Falso)
    cabeça = 0
    Enquanto(ArquivoNãoTotalmenteEnviado()):
        Para i = cabeça até n:
            Se  Igual(enviado[i], Falso):
                dados = Leia1000BytesDoArquivoNaPosicao(i)
                EnviaDadosAoServidor(i, dados)
        
        Enquanto(Igual(recebido,EsperaCofirmaçãoEm(200ms))):
            enviados[recebido] = Verdadeiro
        
        Enquanto(Igual(enviado[cabeça], Verdadeiro)):
            cabeça = cabeça + 1

Alem diso, para garantir que o servidor consiga confirmar ao cliente que os dados chegaram, para a mensagem de Acknolgment foi utilizado o canal de Controle. A partir disso, fica claro que os pacotes são salvos em ordem dentro do arquivo, já que a sequência de Bytes salva no arquivo depende da variável de cabeçalho, variàvel essa que só avança quando os dados nessa posição já chegaram e foram propriamente salvos no arquivo e além disso, é garantido que todo o arquivo será enviado, já que o tamanho do arquivo e Acknolgment são confirmados via Canal de Controle.

O pseudo código apresentado assume um buffer suficientemente grande capaz de suportar em totalidade o arquivo em memória principal, isso no entando é inviável em um cenário real. Em um cenário real, esses parametros, como o tamanho do buffer, dependem de caracteristicas pré definidas entre cliente e servidor, como tamanho maximo de um frame e o tamanho da janela deslizante. Na implementação entregue junto a esse relatório, foi fornecida em src/sliding-window todas as estruturas de dados utilizados pelo cliente e servidor para a utilização do protocolo de Janela Deslizante, foi utilizado tamanho de pacote igual a 1000, tamanho de janela de 32, o que possibilitou que as principais estruturas utilizadas para a Janela Deslizante fossem implementadas fazendo o uso de bit-sets, mais expecificamente 'unsigned long's. Sendo assim, o buffer precisa ter 64x1000 bytes alocados, e o cliente pode enviar até 32x1000 bytes para o servidor por vez.

Implementação:

O código fonte da aplicação foi inteiramente implementado em C, as estruturas de dados utilizadas na comunicação TCP e UDP podem ser encontrados em src/network. O código fonte das entidades principais como a do Cliente e Servidor são encontrados em src/client.c e src.server.c respectivamente. O esse relatório também acompanha um arquivo Makefile que pode ser utilizado para compilar tanto o cliente quanto o servidor.

Para a nomeclatura das funções, foi utilizado palavras em inglês com o padrão:

    "node_da_entidade"_"operação"

Um exemplo pode ser a estrutura "tcp_server", onde para criar essa estrutura pode ser utilizado a função tcp_server_create. 

O Servidor é capaz de receber tanto conexões que utilizam IPv4 quando IPv6, assim como o cliente também é capaz de se comunicar utilizando ambos esses protocolos. 

O Cliente UDP foi implementado de maneira não bloqueante, ou seja, não será esperado nenhum tido de confirmação para que as funções de escrita e leitura retornem, a unica espera para qualquer uma dessas funções é um pequeno tempo de espera, nesse caso definido para 200 millisegundos. Operações que terminem em timeout retornaram valores que significal Falso em testes booleanos. O mesmo não foi necessário para o Cliente TCP, poranto ele segue o padrão bloqueante.

# Conclusão:

Esse trabalho prático foi responsável por me apresentar a conceitos como o protocolo UDP e Janela deslizante, ambos desconhecidos antes da conclusão desse trabalho. Ficou claro a eficiência do protocolo UDP assim como suas fraquezas e possiveis métodos para se superar tais desafios, obviamente a custo de código e tempo do programador, e consequentemente ficou claro as vantagens e desvantagens oferecidas pelo protocolo TCP frente ao protocolo UDP.

# Bibliográfia:
https://en.wikipedia.org/wiki/Sliding_window_protocol
https://www.tutorialspoint.com/sliding-window-protocol