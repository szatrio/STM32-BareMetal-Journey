/**
 * @file main.c
 * @brief Day 86 Flash Controller, Wait States & Sector Erasure
 */

#include <stdint.h>
#include "stm32f401_registers.h"
#include "fault_handler.h"

/* Sector 7 base address (128KB, safe from code region) */
#define FLASH_SECTOR_7_ADDR     (0x08060000UL)
#define FLASH_SECTOR_7_NUM      (7UL)
#define TEST_PATTERN_WORD       (0xCAFEBABEUL)

/* Configure Flash latency and ART Accelerator */
static void flash_configure_acr(void)
{
    /* Enable Prefetch, Instruction Cache, Data Cache, and 2 Wait States */
    FLASH->ACR = FLASH_ACR_PRFTEN |
                 FLASH_ACR_ICEN   |
                 FLASH_ACR_DCEN   |
                 FLASH_ACR_LATENCY_2WS;
}

/* Unlock Flash control register */
static void flash_unlock(void)
{
    if ((FLASH->CR & FLASH_CR_LOCK) != 0UL) {
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

/* Lock Flash control register */
static void flash_lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/* Erase specific Flash sector */
static void flash_erase_sector(uint32_t sector_num)
{
    /* Wait for ongoing Flash operation */
    while ((FLASH->SR & FLASH_SR_BSY) != 0UL) {}

    /* Clear previous error flags */
    FLASH->SR = (FLASH_SR_OPERR  | FLASH_SR_WRPERR | FLASH_SR_PGAERR |
                 FLASH_SR_PGPERR | FLASH_SR_PGSERR);

    /* Configure sector erase and x32 parallelism */
    FLASH->CR &= ~(FLASH_CR_SNB_MASK | FLASH_CR_PSIZE_MASK);
    FLASH->CR |= (FLASH_CR_SER | FLASH_CR_PSIZE_WORD | (sector_num << FLASH_CR_SNB_POS));

    /* Trigger erase operation */
    FLASH->CR |= FLASH_CR_STRT;

    /* Wait until erase completes */
    while ((FLASH->SR & FLASH_SR_BSY) != 0UL) {}

    /* Disable sector erase bit */
    FLASH->CR &= ~FLASH_CR_SER;
}

/* Program 32-bit word into Flash address */
static void flash_program_word(uint32_t address, uint32_t data)
{
    /* Wait for ongoing Flash operation */
    while ((FLASH->SR & FLASH_SR_BSY) != 0UL) {}

    /* Enable programming and x32 parallelism */
    FLASH->CR &= ~FLASH_CR_PSIZE_MASK;
    FLASH->CR |= (FLASH_CR_PG | FLASH_CR_PSIZE_WORD);

    /* Write 32-bit word directly to Flash memory */
    *(volatile uint32_t *)address = data;

    /* Wait until programming completes */
    while ((FLASH->SR & FLASH_SR_BSY) != 0UL) {}

    /* Disable programming bit */
    FLASH->CR &= ~FLASH_CR_PG;
}

int main(void)
{
    volatile uint32_t *target_flash = (volatile uint32_t *)FLASH_SECTOR_7_ADDR;
    uint32_t val_after_erase = 0UL;
    uint32_t val_after_write = 0UL;
    uint32_t acr_status      = 0UL;

    /* 1. Configure Wait States & ART Accelerator */
    flash_configure_acr();
    acr_status = FLASH->ACR;

    /* 2. Unlock Flash controller */
    flash_unlock();

    /* 3. Erase Sector 7 and verify erased state (0xFFFFFFFF) */
    flash_erase_sector(FLASH_SECTOR_7_NUM);
    val_after_erase = *target_flash;

    /* 4. Program test pattern and verify written data (0xCAFEBABE) */
    flash_program_word(FLASH_SECTOR_7_ADDR, TEST_PATTERN_WORD);
    val_after_write = *target_flash;

    /* 5. Re-lock Flash controller for safety */
    flash_lock();

    /* Prevent compiler optimization */
    (void)acr_status;
    (void)val_after_erase;
    (void)val_after_write;

    while (1)
    {
        /* Idle loop */
    }
}
