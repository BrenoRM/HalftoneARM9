//
//  main.c
//  halftone
//
//  Created by Francisco José A. C. Souza
//  Breno Ribeiro Monteiro on 05/04/16.
//  Copyright © 2016 Francisco José A. C. Souza & Breno R. Monteiro. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

unsigned char** matrixAlloc(int width, int height)
{

//   Esta funcao aloca uma matriz de tamanho width x height

    unsigned char **m = malloc(sizeof(unsigned char*) * height);
    
    if (m!= NULL)
    {
        int i = 0;
        
        for(i = 0; i < height; i++)
        {
            m[i] = malloc(sizeof(unsigned char*) * width);
        }
    }
    
    return m;
}

void matrixDealloc(unsigned char **m, int width, int height)
{
//   Desaloca uma matriz dinamicamente alocada com malloc 
//   que possua tamanho width x height

    if(m != NULL)
    {
        int i = 0;
        
        for(i = 0; i < height; i++)
        {
            if (m[i] != NULL)
            {
                free(m[i]);
            }
        }
        
        free(m);
    }
}

void writePGM(unsigned char **matrizHalf, int width, int height, int primeiraVez)
{

// Escreve no disco um arquivo PGM a partir de uma dada matriz de bytes
// Params: 
//  matrizHalf: a matriz de bytes que representa o arquivo PGM
//  saida.pgm: o nome do arquivo a ser escrito no disco
//  width: largura da imagem PGM
//  height: altura da imagem PGM
//
//

    int i, j;
    FILE *file = fopen("saida.pgm", "a");
    if(primeiraVez == 0)
    {
        fprintf(file, "P5\n");
        fprintf(file, "%d %d\n", width, height);
        fprintf(file, "128\n");
    }
    for (i = 0; i < 2; i++){
        for (j = 0; j < width; j++)
        {
            fputc(matrizHalf[i][j], file);
        }
    }
    
    fflush(file);
    fclose(file);
}

unsigned char **buildHalftoneMatrix(unsigned char **matriz_PGM_Original, unsigned char **matriz_PGM_Halftone, int width, int height)
{

//	 Esta funcao cria uma matriz de bytes para a construcao da imagem PGM de 
//	 saida desde programa. Para tanto, essa funcao se serve da seguinte tabela:
//	
//	 Categoria 0: 	000 000
// 					000 000
//
// 	 Categoria 1: 	128 000
//					000 000
//
//	 Categoria 2: 	128 000
//	 				000 128
//
// 	 Categoria 3: 	128 128
//					000 128
//
//	 Categoria 4: 	128 128
//	 				128 128
//				

    unsigned char c;
    int row, col, primeiraVez;
    primeiraVez = 0;

    for (row = 0; row < height; row++)
    {   
        for (col = 0; col < width; col++)
        {

            c = matriz_PGM_Original[row][col];

            if (c >= 0 && c <= 51) 
            {
                    matriz_PGM_Halftone[0][2*col] = 0;
                    matriz_PGM_Halftone[0][2*col + 1] = 0;
                    matriz_PGM_Halftone[1][2*col] = 0;
                    matriz_PGM_Halftone[1][2*col +1] = 0;
                    
            }
            else
            {
                if (c > 51 && c <= 102)
                {
                    matriz_PGM_Halftone[0][2*col] = 128;
                    matriz_PGM_Halftone[0][2*col + 1] = 0;
                    matriz_PGM_Halftone[1][2*col] = 0;
                    matriz_PGM_Halftone[1][2*col +1] = 0;
                }
                else
                {
                    if (c > 102 && c <= 153)
                    {
                        matriz_PGM_Halftone[0][2*col] = 128;
                        matriz_PGM_Halftone[0][2*col + 1] = 0;
                        matriz_PGM_Halftone[1][2*col] = 0;
                        matriz_PGM_Halftone[1][2*col +1] = 128;
                    }
                    else 
                    {
                        if (c > 153 && c < 204)
                        {
                            matriz_PGM_Halftone[0][2*col] = 128;
                            matriz_PGM_Halftone[0][2*col + 1] = 128;
                            matriz_PGM_Halftone[1][2*col] = 0;
                            matriz_PGM_Halftone[1][2*col +1] = 128;
                        }
                        else
                        {
                            matriz_PGM_Halftone[0][2*col] = 128;
                            matriz_PGM_Halftone[0][2*col + 1] = 128;
                            matriz_PGM_Halftone[1][2*col] = 128;
                            matriz_PGM_Halftone[1][2*col +1] = 128;
                        }
                    }
                }
            }
        }
        writePGM(matriz_PGM_Halftone, 2*width, 2*height, primeiraVez);
        primeiraVez = 1;         
    }

}    
	
int main(int argc, const char * argv[])
{
    //file to be read
        
    FILE *file = fopen(argv[1], "r");
    
    printf("\n -> Abrindo arquivo: %s", argv[1]);
    
    if (file != NULL)
    {
        unsigned char buffer = 0;
        
//        Consome cabecalhos inuteis do arquivo pgm
        
        while(getc(file) != '\n');
        while(getc(file) == '#');
        while(getc(file) != '\n');
        
        int width  = 0;
        int height = 0;
        int maxval = 0;
        
//        Realiza leitura de metadados da imagem
//        Como altura, largura e valor maximo de cor.
        
        fscanf(file, "%d", &width);
        fscanf(file, "%d", &height);
        fscanf(file, "%d", &maxval);
        
        printf("\n -> Reading PGM file.");
        printf("\n --> width: %d height: %d", width, height);
        printf("\n --> size: %lu bytes", width * height * sizeof(unsigned char));
        printf("\n");
        
//        Contadores de linha e coluna
        int row, col;
        
//        Alocação de de matriz de bytes da imagem
        unsigned char **m = matrixAlloc(width, height);
        unsigned char **m2 = matrixAlloc(2*width, 2);
        
        getc(file);
        
//        Leitura da imagem PGM
        for(row = 0; row < height; row++)
        {
            for(col = 0; col < width; col++)
            {
                buffer = getc(file);
                m[row][col] = buffer;
            }
        }
        clock_t begin, end;
        double tempo_gasto;
        begin = clock();
//        Calculo das categorias do dithering 2x2
        printf("\n -> Calculando dithering 2x2\n");
//        Criar a matriz de dithering
        buildHalftoneMatrix(m, m2, width, height);
        end = clock();
//        Salva o arquivo no PGM

        printf("\n -> Salvando resultado no arquivo: saida.pgm");
        printf("\n --> width: %d height: %d", 2 * width, 2 * height);
        printf("\n --> size: %lu bytes", 4 * width * height * sizeof(unsigned char));
        printf("\n");
        tempo_gasto = (double)(end - begin)/ CLOCKS_PER_SEC;
        printf("%f\n", tempo_gasto);
        //writePGM(m2, 2*width, 2*height);
//   Fecha arquivos abertos e desaloca matrizes
        
        fclose(file);
        matrixDealloc(m, width, height);
        matrixDealloc(m2, 2*width, 2);
    }
    else
    {
        printf("\n -> File %s not found...\n\n", argv[1]);
    }
    
    return 0;
}