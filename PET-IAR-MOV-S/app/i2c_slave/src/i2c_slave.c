#include "i2c_slave.h" 
#include <stdio.h> 

#include "hardware/i2c.h"

#include "hardware/irq.h"
#include "hardware/dma.h"

uint16_t channel_dma_txI2C ; 
uint16_t channel_dma_rxI2C ; 
uint8_t buffer_rxI2C[BUFFER_RX] ; 
uint16_t buffer_txI2C[BUFFER_TX] ; 
static void irq_dma_rx(void ) ; 
static void irq_dma_tx(void) ; 
static void init_dma_rx() ; 
static void init_dma_tx() ; 
static prt_dma_rx dma_rx ; 
static prt_dma_tx dma_tx ; 



void init_I2C(uint16_t port_a, uint16_t port_b,prt_dma_rx dma_user_rx,prt_dma_tx dma_user_tx ) { 
    ///FIXME:ASSERT_ERROR 
    i2c_init(i2c0,CLK_SPEED) ; 
    //enable dma tx and rx 
    i2c_set_slave_mode(i2c0, true, SLAVE_I2C) ; 
    gpio_set_function(port_a,GPIO_FUNC_I2C ) ; 
    gpio_set_function(port_b,GPIO_FUNC_I2C ) ; 
    gpio_pull_up(port_a) ; 
    gpio_pull_up(port_b) ; 
    //enable dma tx and rx 
    i2c0->hw->dma_cr = 0b11 ; 
    init_dma_rx() ; 
    init_dma_tx() ; 
    dma_rx =  dma_user_rx ; 
    dma_tx =  dma_user_tx ; 
    i2c_get_hw(i2c0)->intr_mask = I2C_IC_INTR_STAT_R_RD_REQ_BITS | I2C_IC_INTR_STAT_R_STOP_DET_BITS | I2C_IC_INTR_STAT_R_TX_ABRT_BITS  ; // | 
    irq_set_exclusive_handler(I2C0_IRQ,irq_dma_tx);
    irq_set_enabled(I2C0_IRQ, true);
}




static void irq_dma_rx(void ){ 
    hw_set_bits(&dma_hw->ints0,1<<channel_dma_rxI2C) ; 
    dma_channel_set_write_addr(channel_dma_rxI2C, buffer_rxI2C,true) ; 
    dma_rx(buffer_rxI2C) ; 

}




static void irq_dma_tx(void) { 
   const uint32_t status = i2c_get_hw(i2c0)->intr_stat;


   if (status & I2C_IC_INTR_STAT_R_TX_ABRT_BITS) { ///BIT 6 iupi ! 
      i2c0->hw->clr_tx_abrt ; 
   }

    if (status & I2C_IC_INTR_STAT_R_STOP_DET_BITS) {
        i2c_get_hw(i2c0)->clr_stop_det;        

    }   
    if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {
        i2c0->hw->clr_rd_req ; 
        dma_tx(buffer_txI2C) ; 
        dma_channel_set_read_addr(channel_dma_txI2C,buffer_txI2C,true) ; 
    }



}


static void init_dma_rx(){ 
    channel_dma_rxI2C =  dma_claim_unused_channel(true) ; 
    dma_channel_config c_rx = dma_channel_get_default_config(channel_dma_rxI2C) ; 
    channel_config_set_transfer_data_size(&c_rx,DMA_SIZE_8) ; 
    channel_config_set_read_increment(&c_rx,false) ; 
    channel_config_set_write_increment(&c_rx,true) ; 
    channel_config_set_dreq(&c_rx, i2c_get_dreq(i2c0,false)) ; 
    dma_channel_configure(
        channel_dma_rxI2C, 
        &c_rx,
        buffer_rxI2C, 
        &i2c_get_hw(i2c0)->data_cmd, 
        BUFFER_RX,
        true  
    ) ; 
    dma_channel_set_irq0_enabled(channel_dma_rxI2C,true) ; 
    dma_irqn_set_channel_enabled(DMA_IRQ_0,channel_dma_rxI2C,true) ; 
    irq_set_exclusive_handler(DMA_IRQ_0,irq_dma_rx) ; 
    irq_set_enabled(DMA_IRQ_0,true) ; 
}


static void init_dma_tx(){
    channel_dma_txI2C =  dma_claim_unused_channel(true) ; 
    dma_channel_config c_tx = dma_channel_get_default_config(channel_dma_txI2C) ; 
    channel_config_set_transfer_data_size(&c_tx,DMA_SIZE_16) ; 
    channel_config_set_read_increment(&c_tx,true) ; 
    channel_config_set_write_increment(&c_tx,false) ; 
    channel_config_set_dreq(&c_tx, i2c_get_dreq(i2c0,true)) ; 
    dma_channel_configure(
        channel_dma_txI2C, 
        &c_tx,
        &i2c_get_hw(i2c0)->data_cmd, //write 
        buffer_txI2C, //read 
        BUFFER_TX,
        true  
    ) ; 

} 

