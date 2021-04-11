# TP2-CG-2020/2

<h2>Integrantes do grupo:</h2>
Henrique Pereira Cristófaro<br>
Vitor Theodoro Rocha Domingues<br>

<h2>Instruções para a compilação e execução</h2>

1. Faça o download da biblioteca SDL-1.2.15.<br>
2. Faça o download de SDL_image-1.2.12 e adicione os arquivos .h, .dll e .lib na pasta do SDL.<br>
3. Faça o download de SDL_mixer-1.2.12 e adicione os arquivos .h, .dll e .lib na pasta do SDL.<br>
4. No Codeblocks clique em Settings->Compiler->Search directories-> Add->escolha onde a biblioteca SDL está salva.<br>
5. Botão direito do mouse no projeto testeImage e clique em Properties->Project's build options->Linker settings e na área de Other linker options adicione  seguinte " -lmingw32 -lSDLmain -lSDL -lopengl32 -lglu32 -lSDL_image
-lSDL_mixer".



 
<h2>Bibliotecas Utilizadas:</h2>

SDL, SDL_image, SDL_mixer:<br> 
sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev <br> 
SOIL: <br> 
sudo apt-get install libsoil-dev <br> 

<h2>Lista de itens adicionais implementados em seu jogo</h2>
- Reshape<br>

- Inimigos vão ficando mais devagar quando em menor número<br>
