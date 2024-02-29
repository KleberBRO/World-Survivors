#include <SFML/Graphics.h>
#include <SFML/Audio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define ALTURA 824
#define LARGURA 1280
int frames = 0;
bool pause = true, gameover = false;

typedef struct
{
    int dano;
   float velocidadeDeMove;
   int direcao;
   int vida;
   sfVector2f posicao;
   sfIntRect tRect;

} AtributosPerson;

typedef struct 
{
    sfVector2f posicaoAtual;
    float posicaoFinal;
    bool estado;
    sfSprite *sprite;
    sfIntRect tRect;
    bool direcao;

    struct atributosBala *next;
}atributosBala; //dinâmico para caso dê tempo de implementar os níveis

typedef struct 
{
    atributosBala *head;
} ListaBalas;


typedef struct
{
    int velocidade;
    int direcao;
    int dano;
    float vida;
    sfSprite *sprite;
    sfIntRect tRect;
    sfVector2f posicao;
    sfVector2f sentido;

    struct Inimigo *next;
} Inimigo;

typedef struct 
{
    Inimigo *head;
} ListaInimigos;

Inimigo *addInimigo(ListaInimigos *lista, sfTexture *textura, sfVector2f posicao){

    Inimigo *novoInimigo = (Inimigo *)malloc(sizeof(Inimigo));

    int x, y;
    do
    {
        x = (posicao.x-LARGURA/2)-100 + rand() % (LARGURA+150);
    } while (x > posicao.x-LARGURA/2 && x < posicao.x+LARGURA/2);
    do
    {
        y = (posicao.y-ALTURA/2)-100 + rand() % (ALTURA+150);
    } while (y > posicao.y-ALTURA/2 && y < posicao.y+ALTURA/2);
    
    

    novoInimigo->posicao = (sfVector2f){x, y};
    novoInimigo->vida = 2;
    novoInimigo->direcao = 1;
    novoInimigo->dano = 0;
    novoInimigo->sprite = sfSprite_create();
    novoInimigo->tRect = (sfIntRect){0,77,50,44};
    sfSprite_setTexture(novoInimigo->sprite, textura, sfTrue);
    sfSprite_scale(novoInimigo->sprite, (sfVector2f){2,2});
    sfSprite_setTextureRect(novoInimigo->sprite, novoInimigo->tRect);
    sfSprite_setPosition(novoInimigo->sprite, novoInimigo->posicao);

    novoInimigo->next = lista->head;
    lista->head = novoInimigo;

    return novoInimigo;
}

void renderInimigos(ListaInimigos *lista, sfRenderWindow *window, sfVector2f posicaoPlayer) {
    Inimigo *atual = lista->head;

    while (atual != NULL)
    {

        if (!pause){
            // Atualize a animação de movimento

            if (frames%10 == 9) {
                atual->tRect.left += 73;
                if (atual->tRect.left >= 779) 
                    atual->tRect.left = 0;
            } else {
            if (frames%10 == 9) {
                atual->tRect.left += 73;
                    if (atual->tRect.left >= 779) {
                        atual->tRect.left = 0;
                    }
                }
            }

            atual->sentido.x = posicaoPlayer.x - atual->posicao.x;
            atual->sentido.y = posicaoPlayer.y - atual->posicao.y;

            float distancia = sqrt(pow(atual->sentido.x, 2) + pow(atual->sentido.y, 2));

            if (atual->sentido.x > 0){
                atual->tRect.top = 157;
                if(atual->dano > 0){
                    atual->tRect.top = 286;
                    atual->tRect.left = 0;
                    atual->dano--;
                }
            }else {
                atual->tRect.top = 204;
                if(atual->dano > 0){
                    atual->tRect.top = 333;
                    atual->tRect.left = 0;
                    atual->dano--;
                }
            }

            if (distancia != 0)
            {
                atual->posicao.x += atual->sentido.x*100/distancia*0.01;
                atual->posicao.y += atual->sentido.y*100/distancia*0.01;
            }
        }

        sfSprite_setTextureRect(atual->sprite, atual->tRect);
        sfSprite_setPosition(atual->sprite ,atual->posicao);
        sfRenderWindow_drawSprite(window, atual->sprite, NULL);

        atual = atual->next;
    }
}

atributosBala *addBala(ListaBalas *lista, sfTexture *textura, sfVector2f playerPosicao, int direcaoBala){
    atributosBala *novaBala = (atributosBala *)malloc(sizeof(atributosBala));
    if (novaBala == NULL){    
        printf("erro de alocação");
        return NULL;
    }

    novaBala->posicaoAtual = playerPosicao;
    if (direcaoBala == 1){
        novaBala->posicaoFinal = playerPosicao.x + 600;
        novaBala->direcao = true;
    }else{
        novaBala->posicaoFinal = playerPosicao.x - 600;
        novaBala->direcao = false;
    }
    
    novaBala->estado = true;
    novaBala->sprite = sfSprite_create();
    novaBala->tRect = (sfIntRect){0,253,28,28};
    sfSprite_setTexture(novaBala->sprite, textura, NULL);
    sfSprite_setTextureRect(novaBala->sprite, novaBala->tRect);
    sfSprite_setPosition(novaBala->sprite,novaBala->posicaoAtual);

    novaBala->next = lista->head;
    lista->head = novaBala;

    return novaBala;
}

void atirarBala(ListaBalas *lista, int direcaoBala, bool *addBala, sfVector2f playerPosicao){
    atributosBala *atual = lista->head;

       while (atual != NULL) {
        if (atual->estado == false) {
            atual->posicaoAtual = playerPosicao;
            if (direcaoBala == 1) {
                atual->posicaoFinal = playerPosicao.x + 500;
                atual->direcao = true;
            } else {
                atual->posicaoFinal = playerPosicao.x - 500;
                atual->direcao = false;
            }
            
            atual->estado = true;
            return;
        }

        atual = atual->next;
        }

        *addBala = true;
}

void renderBalas(ListaBalas *lista, sfRenderWindow *window){
    atributosBala *atual = lista->head;

    while (atual != NULL){
        if (atual->estado) {       
            if (atual->direcao == false) {// esquerda
                if (atual->posicaoAtual.x > atual->posicaoFinal){
                    atual->posicaoAtual.x -= 5;
                } else{
                    atual->estado = false;
                    sfSprite_setPosition(atual->sprite, (sfVector2f){0,0});
                }
            } else { //direita
                if (atual->posicaoAtual.x < atual->posicaoFinal){
                    atual->posicaoAtual.x += 5;
                } else{
                    atual->estado = false;
                    sfSprite_setPosition(atual->sprite, (sfVector2f){0,0});
                }
            }

            sfSprite_setPosition(atual->sprite, atual->posicaoAtual);
            sfRenderWindow_drawSprite(window, atual->sprite, NULL);
        }
        
        atual = atual->next;
    }
    
}

void destroyListaBalas(ListaBalas *lista) {
    atributosBala *atual = lista->head;
    while (atual != NULL){
        atributosBala *prox = atual->next;
        sfSprite_destroy(atual->sprite);
        free(atual);
        atual = prox;
    }
    lista->head = NULL;
    
}


void hoverMouse(sfFloatRect *rect_play, sfSprite *sPlay, sfVector2i posMouse) {
    if (sfFloatRect_contains(rect_play, posMouse.x, posMouse.y)) {
        sfSprite_setScale(sPlay, (sfVector2f){0.4, 0.4});
        sfSprite_setPosition(sPlay, (sfVector2f){(float)(LARGURA / 2) - 125.2, (float)(ALTURA / 2) - 115});
    } else {
        sfSprite_setScale(sPlay, (sfVector2f){0.3, 0.3});
        sfSprite_setPosition(sPlay, (sfVector2f){(float)(LARGURA / 2) - 93.9, (float)(ALTURA / 2) - 93.9});
    }
    // atualiza o rect do play
    *rect_play = sfSprite_getGlobalBounds(sPlay);
}

void moverPlayer(sfSprite *sPlayer, sfVector2f *posicao, sfIntRect *rect, float velocidade, int *direcao, int *direcaoBala){
    bool parado = true;
    if (sfKeyboard_isKeyPressed(sfKeyW)) {
        if (posicao->y > 35){
            posicao->y -= 3;
            parado = false;
        }
    }
    if (sfKeyboard_isKeyPressed(sfKeyS)) {
        if (posicao->y < 4120){
            posicao->y += 3;
            parado = false;
        }
    }
    if (sfKeyboard_isKeyPressed(sfKeyA)) {
        if (posicao->x > 65){
            posicao->x -= 3;
            *direcao = 0;
            parado = false;
        }
    }
    if (sfKeyboard_isKeyPressed(sfKeyD)) {
        if (posicao->x < 6200){
            posicao->x += 3;
            *direcao = 1;
            parado = false;
        }    
    }


    if (sfKeyboard_isKeyPressed(sfKeyRight)) {
        *direcaoBala = 1;            
    }else if (sfKeyboard_isKeyPressed(sfKeyLeft)) {
        *direcaoBala = 2;        
    } 

    if (parado) {
        // Atualize a animação de movimento
        if (frames%6 == 5) {
            if (*direcao == 1) {
                rect->top = 0;
            } else {
                rect->top = 38;
            }

            rect->left += 120;
            if (rect->left >= 1102) 
                rect->left = 0;
        }

    } else {
        if (frames%6 == 5) {
            if (*direcao == 1) {
                rect->top = 76;
                
            } else {
                rect->top = 115;
            }

            rect->left += 120;
            if (rect->left >= 1102) {
                rect->left = 0;
            }
        }
        
    }
    
    
    sfSprite_setTextureRect(sPlayer, *rect);
    sfSprite_setPosition(sPlayer, *posicao);

    if (frames >= 60) {
        frames = 0;
    } else { 
        frames++;
    }
}

void destroyInimigo(ListaInimigos *lista, sfVector2f alvo){
    Inimigo *anterior = NULL;
    Inimigo *atual = lista->head;

    //encontrar o inimigo na lista
    while (atual != NULL && atual->posicao.x != alvo.x & atual->posicao.y != alvo.y){
        anterior = atual;
        atual= atual->next;
    }

    if (atual != NULL) {
        //remover o inimigo
        if(anterior != NULL) {
            anterior->next = atual->next;
        }else{
            lista->head = atual->next;
        }

        sfSprite_destroy(atual->sprite);
        free(atual);
    }
    

}

void colisaoCheck(ListaBalas *listaBalas, ListaInimigos *listaInimigos, AtributosPerson *player, sfFloatRect hitboxPlayer, sfSound *hit){
    Inimigo *InimigoAnterior = NULL;
    Inimigo *inimigoAtual = listaInimigos->head;
    
    while (inimigoAtual != NULL) {
        sfFloatRect hitboxInimigo = sfSprite_getGlobalBounds(inimigoAtual->sprite);
        atributosBala *balaAtual = listaBalas->head;
        while (balaAtual != NULL) {
            if (balaAtual->estado == true){
                sfFloatRect hitboxBala = sfSprite_getGlobalBounds(balaAtual->sprite);

                if (sfFloatRect_intersects(&hitboxInimigo, &hitboxBala, NULL)) {
                    sfSound_play(hit);
                    inimigoAtual->vida--;
                    inimigoAtual->dano = 5;
                    balaAtual->estado = false;
                    sfSprite_setPosition(balaAtual->sprite, (sfVector2f){0,0});
                }
            }

            balaAtual = balaAtual->next;
        }

        if (sfFloatRect_intersects(&hitboxInimigo, &hitboxPlayer, NULL) && player->dano <= 0) {
            player->vida--;
            player->dano = 30;
        }
        

        if (inimigoAtual->vida <= 0){
            destroyInimigo(listaInimigos, inimigoAtual->posicao);
        }
        
        inimigoAtual = inimigoAtual->next;
    }
}

void checkGameover(AtributosPerson *player){
    if (player->dano > 0){
        if (player->direcao == 1)
        {
            if(player->dano % 5 == 0){
                player->tRect.left = 120;
                player->tRect.top = 286;
            }
        } else {
            if(player->dano % 5 == 0){
                player->tRect.left = 120;
                player->tRect.top = 333;
            }
        }
        player->dano--;
        
    }
    if(player->dano<=0){
        gameover = true;
    }
    
}

int main() {
    srand(time(NULL));
    // Criação da janela
    sfVideoMode mode = {LARGURA, ALTURA, 32};
    sfRenderWindow* window;

    window = sfRenderWindow_create(mode, "survivors", sfDefaultStyle, NULL);
    sfWindow_setFramerateLimit(window, 60);

    //tempo de jogo
    sfClock *clock = sfClock_create();

    //Botão de play
    sfTexture *tPlay = sfTexture_createFromFile("CSFML_projeto/imgs/play.png", NULL);
    sfSprite *sPlay = sfSprite_create();
    sfSprite_setTexture(sPlay, tPlay, 0);
    sfSprite_setScale(sPlay,(sfVector2f){0.3,0.3});
    sfSprite_setPosition(sPlay, (sfVector2f){(float)(LARGURA/2)-93.9, (float)(ALTURA/2)-93.9});
    sfFloatRect rect_play = sfSprite_getGlobalBounds(sPlay);

    //Background do menu
    sfTexture *tBack = sfTexture_createFromFile("CSFML_projeto/imgs/background.jpg", NULL);
    sfSprite *sBack = sfSprite_create();
    sfSprite_setTexture(sBack, tBack, 0);
    sfSprite_setScale(sBack,(sfVector2f){0.75,0.75});


    //mapa do jogo
    sfTexture *tMapa = sfTexture_createFromFile("CSFML_projeto/imgs/Mapa.jpg", NULL);
    sfSprite *sMapa = sfSprite_create();
    sfSprite_setTexture(sMapa, tMapa, 0);

    // Define a view (câmera)
    sfView *view = sfView_create();
    sfView_setCenter(view, (sfVector2f){LARGURA/2, ALTURA/2});
    sfView_setSize(view, (sfVector2f){LARGURA, ALTURA});

    // Aplica a view à janela
    sfRenderWindow_setView(window, view);

    //Criação da fonte
    sfFont *fonte = sfFont_createFromFile("CSFML_projeto/imgs/pixelFont.ttf");
    
    //gerar texto de tempo
    sfText *txtTempo = sfText_create();
    sfText_setFont(txtTempo, fonte);
    sfText_setCharacterSize(txtTempo, 38);
    sfText_setFillColor(txtTempo, sfWhite);

    //personagem
    AtributosPerson player = {0,3,1,10,3200,2060,0,0,30,38};
    sfTexture *tSprites = sfTexture_createFromFile("CSFML_projeto/imgs/personagem.png", NULL);
    sfSprite *sPlayer = sfSprite_create();
    sfSprite_setTexture(sPlayer, tSprites, sfTrue);
    sfSprite_scale(sPlayer, (sfVector2f){2,2});
    sfSprite_setTextureRect(sPlayer, player.tRect);
    sfSprite_setPosition(sPlayer, player.posicao);

    //Criaçao da lista de inimigos
    ListaInimigos listaInimigos;
    listaInimigos.head = NULL;

    //Criação da lista de balas
    ListaBalas listaBalas;
    listaBalas.head = NULL;

    //criação do buffer de som
    sfSoundBuffer *bufferP = sfSoundBuffer_createFromFile("CSFML_projeto/sons/shoot.wav");
    sfSound *projetilSound = sfSound_create();
    sfSound_setBuffer(projetilSound, bufferP);
    sfSound_setVolume(projetilSound, 50);
    sfSoundBuffer *bufferH = sfSoundBuffer_createFromFile("CSFML_projeto/sons/hit.wav");
    sfSound *HitSound = sfSound_create();
    sfSound_setBuffer(HitSound, bufferH);
    
    bool iniciar = false;
    bool balaAdd = false;
    float tempoPausado = 0, segundos = 0;
    int qntd_inimigos, spawn_inimigos = 0, cooldownBala = 0, direcaoBala = 0;

//******************************Inicio do Jogo**************************//
    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        sfVector2i posMouse = sfMouse_getPositionRenderWindow(window);

        sfTime time = sfClock_getElapsedTime(clock);

        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

            //caso o jogo seja minimizado
            if (event.type == sfEvtLostFocus) {
                pause = true; 
            }        


            if(event.mouseButton.button == sfMouseLeft){
                //Quando o botão de Play for clicado
                if(sfFloatRect_contains(&rect_play, posMouse.x, posMouse.y ) && !iniciar){
                    iniciar = true;
                    pause = false;
                    sfSprite_destroy(sPlay);
                    sfTexture_destroy(tPlay);
                    tempoPausado = sfTime_asSeconds(time);
                }
            }

            //Pausar o jogo usando ESC
            if (event.type == sfEvtKeyPressed) {
                if(event.key.code == sfKeyEscape){
                    pause = !pause;
                    printf("%f\n", tempoPausado);
                    if (!pause) {
                        tempoPausado = sfTime_asSeconds(time) - segundos;
                    }                 
                }
            }
            
        }
        
    
        if (!pause && iniciar) {
            cooldownBala++;
            moverPlayer(sPlayer, &player.posicao, &player.tRect, player.velocidadeDeMove, &player.direcao, &direcaoBala);
            //centralizar "camera" no player
            sfView_setCenter(view, (sfVector2f){player.posicao.x+(player.tRect.width/2), player.posicao.y+(player.tRect.height/2)});

            segundos = sfTime_asSeconds(time) - tempoPausado;
            qntd_inimigos = 2;

            if (cooldownBala > 5 && direcaoBala != 0) {
                atirarBala(&listaBalas, direcaoBala, &balaAdd, player.posicao);
                if (balaAdd){
                    addBala(&listaBalas, tSprites, player.posicao, direcaoBala);
                    balaAdd = false;
                }
                sfSound_play(projetilSound);
                cooldownBala = 0;
            }
            direcaoBala = 0;
            

            if (frames%60 == 59) {
                spawn_inimigos++;
                if (spawn_inimigos == 3)  {
                    for (int i = 0; i < qntd_inimigos; i++)
                    {
                        addInimigo(&listaInimigos, tSprites, player.posicao);
                    }
                    spawn_inimigos=0;
                }
                
            }
            sfFloatRect hitboxPlayer = sfSprite_getGlobalBounds(sPlayer);
            colisaoCheck(&listaBalas, &listaInimigos, &player, hitboxPlayer, HitSound);

        }
        checkGameover(&player);

        char timeString[128];
        sprintf(timeString, "%i:%02i", (int)floor(segundos/60), (int)segundos%60);
        sfText_setString(txtTempo, timeString);
        sfText_setPosition(txtTempo, (sfVector2f){player.posicao.x, player.posicao.y-ALTURA/2.2});

        sfRenderWindow_setView(window, view);
        sfRenderWindow_clear(window, sfBlack);

        //Draw Menu
        if (!iniciar){
            hoverMouse(&rect_play, sPlay, posMouse);
            sfRenderWindow_drawSprite(window, sBack, NULL);
            sfRenderWindow_drawSprite(window, sPlay, NULL);
        }else{
            sfRenderWindow_drawSprite(window, sMapa, NULL);
            sfRenderWindow_drawSprite(window, sPlayer, NULL);
            renderInimigos(&listaInimigos, window, player.posicao);
            renderBalas(&listaBalas, window);
        }
        sfRenderWindow_drawText(window, txtTempo, NULL);

        sfRenderWindow_display(window);

    }

    sfRenderWindow_destroy(window);

    sfClock_destroy(clock);
    sfFont_destroy(fonte);
    sfText_destroy(txtTempo);
    sfSound_destroy(projetilSound);
    sfSoundBuffer_destroy(bufferP);
    sfSound_destroy(HitSound);
    sfSoundBuffer_destroy(bufferH);

    if (!iniciar) {
        sfSprite_destroy(sPlay);
        sfTexture_destroy(tPlay);
    }
    
    sfTexture_destroy(tBack);
    sfTexture_destroy(tMapa);
    sfTexture_destroy(sMapa);
    sfTexture_destroy(tSprites);
    destroyListaBalas(&listaBalas);
    sfSprite_destroy(sBack);
    sfSprite_destroy(sPlayer);

    sfView_destroy(view);

    //destroyListaInimigos(&listaInimigos);
    

    return 0;
}