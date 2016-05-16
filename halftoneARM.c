//
//  main.c
//  halftone
//
//  Created by Francisco José A. C. Souza
//  Breno Ribeiro Monteiro on 05/04/16.
//  Copyright © 2016 Francisco José A. C. Souza. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

unsigned char** matrixAlloc(int width, int height);
void matrixDealloc(unsigned char **m, int width, int height);
void writePGM(unsigned char **m, int width, int height);

void calculateHalftoneCategories(unsigned char **m, int width, int height);
unsigned char **buildHalftoneMatrix(unsigned char **m, int width, int height);

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
        calculateHalftoneCategories(m, width, height);
        end = clock();
//        Criar a matriz de dithering
        unsigned char **m2 = buildHalftoneMatrix(m, width, height);
        
//        Salva o arquivo no PGM

        printf("\n -> Salvando resultado no arquivo: saida.pgm");
        printf("\n --> width: %d height: %d", 2 * width, 2 * height);
        printf("\n --> size: %lu bytes", 4 * width * height * sizeof(unsigned char));
        printf("\n");
        tempo_gasto = (double)(end - begin)/ CLOCKS_PER_SEC;
        printf("%f\n", tempo_gasto);
        writePGM(m2, 2*width, 2*height);
// 	 Fecha arquivos abertos e desaloca matrizes
        
        fclose(file);
        matrixDealloc(m, width, height);
        matrixDealloc(m2, 2*width, 2*height);
    }
    else
    {
        printf("\n -> File %s not found...\n\n", argv[1]);
    }
    
    return 0;
}

void calculateHalftoneCategories(unsigned char **m, int width, int height)
{
    unsigned char c;
    int row, col;
	
//	 Calcula as categorias do dithering 2x2 dividindo o intervalo de
//	 0..255 segundo a seguinte tabela.
//
//	 000..051: categoria 0
//	 052..102: categoria 1
//	 103..153: categoria 2
// 	 154..204: categoria 3
//	 205..255: categoria 4	
//
//

    for(row = 0; row < height; row++)
    {
        for (col = 0; col < width; col++)
        {
            c = m[row][col];
            
            if (c >= 0 && c <= 51)
            {
                m[row][col] = 0;
            }
            else
            {
                if (c > 51 && c <= 102)
                {
                    m[row][col] = 1;
                }
                else
                {
                    if (c > 102 && c <= 153)
                    {
                        m[row][col] = 2;
                    }
                    else
                    {
                        if (c > 153 && c < 204)
                        {
                            m[row][col] = 3;
                        }
                        else
                        {
                            m[row][col] = 4;
                        }
                    }
                }
            }
        }
    }

}

unsigned char **buildHalftoneMatrix(unsigned char **m, int width, int height)
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

    int row, col;
    
    unsigned char **m2 = matrixAlloc(width*2, height*2);
    
    for(row = 0; row < height; row ++)
    {
        for (col = 0; col < width; col++)
        {
            switch (m[row][col]) {
                case 0:
                    m2[2*row][2*col] = 0;
                    m2[2*row][2*col + 1] = 0;
                    m2[2*row + 1][2*col] = 0;
                    m2[2*row + 1][2*col +1] = 0;
                    
                    break;
                    
                case 1:
                    m2[2*row][2*col] = 128;
                    m2[2*row][2*col + 1] = 0;
                    m2[2*row + 1][2*col] = 0;
                    m2[2*row + 1][2*col +1] = 0;
                    
                    break;
                    
                case 2:
                    m2[2*row][2*col] = 128;
                    m2[2*row][2*col + 1] = 0;
                    m2[2*row + 1][2*col] = 0;
                    m2[2*row + 1][2*col +1] = 128;
                    
                    break;
                    
                case 3:
                    m2[2*row][2*col] = 128;
                    m2[2*row][2*col + 1] = 128;
                    m2[2*row + 1][2*col] = 0;
                    m2[2*row + 1][2*col +1] = 128;
                    
                    break;
                    
                    
                default:
                    m2[2*row][2*col] = 128;
                    m2[2*row][2*col + 1] = 128;
                    m2[2*row + 1][2*col] = 128;
                    m2[2*row + 1][2*col +1] = 128;
                    
                    break;
            }
        }
    }
    
    return m2;
    
}

unsigned char** matrixAlloc(int width, int height)
{

//	 Esta funcao aloca uma matriz de tamanho width x height

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
// 	 Desaloca uma matriz dinamicamente alocada com malloc 
//	 que possua tamanho width x height

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

void writePGM(unsigned char **m, int width, int height)
{

// Escreve no disco um arquivo PGM a partir de uma dada matriz de bytes
// Params: 
// 	m: a matriz de bytes que representa o arquivo PGM
//  filename: o nome do arquivo a ser escrito no disco
//  width: largura da imagem PGM
//  height: altura da imagem PGM
//
//

    int i, j;
    FILE *file = fopen("saida.pgm", "a");
    fprintf(file, "P5\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "128\n");
    for (i = 0; i < height; i++){
        for (j = 0; j < width; j++)
        {
            fputc(m[i][j], file);
        }
    }
    
    fflush(file);
    fclose(file);
}