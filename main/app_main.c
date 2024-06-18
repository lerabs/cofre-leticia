//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                     _                //
//        _ _  _    _  . .  _  .   _    .   _     _   _   _    _   _   _  .  .  _   _  _   _     .      //
//         |  | |  |_| |\| |_| |\ |_|   |\ |_|   |_| |_| | |  |   |_| |_| |\/| |_| |  |_| | |   /|      //    
//       |_|  |_|  |\  | | | | |/ | |   |/ | |   |   |\  |_|  |_| |\  | | |  | | | |_ | | |_|   _|_     //
//                                                                                     /                //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*   Programa básico para controle da placa durante a Jornada da Programação 1
*   Permite o controle das entradas e saídas digitais, entradas analógicas, display LCD e teclado. 
*   Cada biblioteca pode ser consultada na respectiva pasta em componentes
*   Existem algumas imagens e outros documentos na pasta Recursos
*   O código principal pode ser escrito a partir da linha 86
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ioplaca.h"  
#include "lcdvia595.h"
#include "driver/adc.h"
#include "hcf_adc.h"
#include "MP_hcf.h"  
#include "esp_err.h"
// Área das macros
//-----------------------------------------------------------------------------------------------------------------------

#define num1 1243

// Área de declaração de variáveis e protótipos de funções
//-----------------------------------------------------------------------------------------------------------------------

static const char *TAG = "Placa";
static uint8_t entradas, saidas = 0; //variáveis de controle de entradas e saídas

int ctrl = 0;
int senha = 0;
int tl = 0;
int coluna = 0;
char operador;
char tecla;
char exibir[40];
uint32_t adcvalor = 0;
int seguranca = 0;


// Função sempre ativa(?)
//-----------------------------------------------------------------------------------------------------------------------


// Funções e ramos auxiliares
//-----------------------------------------------------------------------------------------------------------------------
void abrir () 
{
     lcd595_write (1, 0, "Abrindo Cofre");
     for (int i = 0; i < 1964 ; i = adcvalor)
     {
        hcf_adc_ler (&adcvalor);
        rotacionar_DRV (1,10, saidas);
        seguranca ++;
        if (seguranca > 15)break;
     }
     seguranca = 0;
}

void fechar ()
{
     lcd595_write (1, 0, "Fechando Cofre");
     hcf_adc_ler (&adcvalor);
     for (int i = adcvalor; i > 400  ; i = adcvalor)
     {
        hcf_adc_ler (&adcvalor);
        rotacionar_DRV (0,10, saidas);
        seguranca ++;
        if (seguranca > 15)break;
     }
     seguranca = 0;
}


// Programa Principal
//-----------------------------------------------------------------------------------------------------------------------

void app_main(void)
{
    MP_init(); // configura pinos do motor
    // a seguir, apenas informações de console, aquelas notas verdes no início da execução
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    /////////////////////////////////////////////////////////////////////////////////////   Inicializações de periféricos (manter assim)
    
    // inicializar os IOs e teclado da placa
    ioinit();      
    entradas = io_le_escreve(saidas); // Limpa as saídas e lê o estado das entradas

    // inicializar o display LCD 
    lcd595_init();
    lcd595_write(1,1,"    Cofre da   ");
    lcd595_write(2,1," Lerabs ");
    
    // Inicializar o componente de leitura de entrada analógica
    esp_err_t init_result = hcf_adc_iniciar();
    if (init_result != ESP_OK) {
        ESP_LOGE("MAIN", "Erro ao inicializar o componente ADC personalizado");
    }


    // inicia motor
    DRV_init(6, 7);

    //delay inicial
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    lcd595_clear();

    /////////////////////////////////////////////////////////////////////////////////////   Periféricos inicializados
    hcf_adc_ler(&adcvalor);

    if(adcvalor > 400)
       {
           hcf_adc_ler(&adcvalor);
           while(adcvalor > 400)
           {
                hcf_adc_ler(&adcvalor);
                rotacionar_DRV(0, 11, saidas);
           }
      }

    /////////////////////////////////////////////////////////////////////////////////////   Início do ramo principal                    
   while(1)
    {
       hcf_adc_ler(&adcvalor);
    

       tecla = le_teclado();

        if(tecla>='0' && tecla <='9')
        {
          if(senha == 0)
            {
                senha = senha * 10 + tecla - '0';
                ctrl = ctrl + 1;
            }
        }

        

        lcd595_write(1,0, "Insira a senha");
    
        switch (ctrl)
            {   case 0: lcd595_write(2, 0, "[ ] [ ] [ ] [ ]");
                    break;
                case 1: lcd595_write(2, 0, "[*] [ ] [ ] [ ]");
                    break;
                case 2: lcd595_write(2, 0, "[*] [*] [ ] [ ]");
                    break;
                case 3: lcd595_write(2, 0, "[*] [*] [*] [ ]");
                    break;
                case 4: lcd595_write(2, 0, "[*] [*] [*] [*]");
                    break;
                default: lcd595_write(2, 0, "erro");
                    break;
            }   

          
        if(tecla == 'C')
        {
           senha = 0;
           tl = 0; 
        }
            

        if(tl == 4)
        {
            if(senha == num1)
            {
                int interrompe = 10;

                hcf_adc_ler(&adcvalor);
            
                while(adcvalor <= 1964 && interrompe > 0)
                {
                hcf_adc_ler(&adcvalor);

                lcd595_clear();
                lcd595_write(1,0, "Abrindo Cofre");

                rotacionar_DRV(1, 11, saidas);
                
                interrompe--;
                }
                
                if(adcvalor == 1964 || interrompe == 0)
                {
                    vTaskDelay(15000 / portTICK_PERIOD_MS);

                     hcf_adc_ler(&adcvalor);

                    while(adcvalor >= 400 && interrompe < 11)
                    {  
                        hcf_adc_ler(&adcvalor);

                        lcd595_clear();
                        lcd595_write(1,0, "Fechando Cofre");

                        rotacionar_DRV(0, 11, saidas);

                        interrompe++;
                    }
                tl = 0;
                senha = 0;
                }

            }

            else
            {
                lcd595_clear();
                lcd595_write(1,0, "Senha Incorreta.");
                lcd595_write(2,1, "Tente novamente.");
              
                tl = 0;
                senha = 0;
            }
        
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
       
    }     
        
        vTaskDelay(100 / portTICK_PERIOD_MS);

    // caso erro no programa, desliga o módulo ADC
    hcf_adc_limpar(); 
}
