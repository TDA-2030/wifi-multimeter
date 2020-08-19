/**************************************************************************/
/*!
    @file     ADS1x1x.c
    @author   CPV (Elektor)
    @license  BSD (see license.txt)

    Driver for the ADS101x/ADS111x Sigma-Delta Analog/Digital Converters.

    This started out as a port of the Adafruit library, but ended
    up largely rewritten.

    v1.0 - First release
*/
/**************************************************************************/


#include "i2c_bus.h"
#include "ads1x1x.h"

static i2c_bus_t *g_i2c_bus = NULL;

void inline delay(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

/**************************************************************************/
/*!
    @brief  Writes 16 bits to the specified destination register.
*/
/**************************************************************************/
void ADS1x1x_write_register(uint8_t i2c_address, uint8_t reg, uint16_t value)
{
  i2c_set_address(g_i2c_bus, i2c_address);
  uint8_t dat[5];
  dat[0] = reg;
  dat[1] = (value>>8);
  dat[2] = (value & 0xFF);
  i2c_master_write_slave(g_i2c_bus, dat, 3);
}

/**************************************************************************/
/*!
    @brief  Read 16 bits from the specified destination register.
*/
/**************************************************************************/
uint16_t ADS1x1x_read_register(uint8_t i2c_address, uint8_t reg)
{
  i2c_set_address(g_i2c_bus, i2c_address);
  uint8_t dat[5];
  dat[0] = reg;
  i2c_master_write_slave(g_i2c_bus, dat, 1);
  i2c_master_read_slave(g_i2c_bus, dat, 2);

  return ((dat[0] << 8) | dat[1]); 
}


/**************************************************************************/
/*!
    @brief  Initialise a user-provided ADS1X15 configuration structure.
            The user must provide a valid pointer to an empty
            ADS1x1x_config_t structure.
*/
/**************************************************************************/
uint8_t ADS1x1x_init(ADS1x1x_config_t *p_config, ADS1x1x_chip_t chip, uint8_t i2c_address, ADS1x1x_mux_t input, ADS1x1x_pga_t gain)
{
  uint8_t result = 0;

  if (p_config!=0)
  {
    i2c_config_t i2c_conf = DEFAULT_I2C_BUS_MASTER(4, 5);
    g_i2c_bus = i2c_bus_create(I2C_NUM_0, &i2c_conf);
    // Set generic parameters.
    p_config->chip = chip;
    p_config->i2c_address = i2c_address;
    p_config->gain = gain;

    // Set configuration bits for default operation.
    p_config->config = 0;
    ADS1x1x_set_os(p_config,OS_SINGLE);
    ADS1x1x_set_multiplexer(p_config,input);
    ADS1x1x_set_pga(p_config,gain);
    ADS1x1x_set_mode(p_config,MODE_SINGLE_SHOT);
    if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
    {
      ADS1x1x_set_data_rate(p_config,DATA_RATE_ADS101x_1600);
    }
    else
    {
      ADS1x1x_set_data_rate(p_config,DATA_RATE_ADS111x_128);
    }
    ADS1x1x_set_comparator_mode(p_config,COMPARATOR_MODE_TRADITIONAL);
    ADS1x1x_set_comparator_polarity(p_config,COMPARATOR_POLARITY_ACTIVE_LO);
    ADS1x1x_set_comparator_latching(p_config,COMPARATOR_NON_LATCHING);
    ADS1x1x_set_comparator_queue(p_config,COMPARATOR_QUEUE_NONE);
    
    result = 1;
  }

  return result;
}


/**************************************************************************/
/*!
    @brief  Start an ADC conversion cycle.
            The user must provide a valid pointer to a
            correctly filled ADS1x1x_config_t structure.
*/
/**************************************************************************/
void ADS1x1x_trigger_conversion(ADS1x1x_config_t *p_config)
{
  // Write configuration to the ADC.
  ADS1x1x_set_os(p_config, OS_SINGLE);
  ADS1x1x_write_register(p_config->i2c_address,ADS1x1x_REG_POINTER_CONFIG,p_config->config);
}

bool ADS1x1x_isConversionReady(ADS1x1x_config_t *p_config)
{
    uint16_t reg = ADS1x1x_read_register(p_config->i2c_address, ADS1x1x_REG_POINTER_CONFIG);
    return (reg & ADS1x1x_REG_CONFIG_OS_MASK)?1:0;
}

bool ADS1x1x_pollConversion(ADS1x1x_config_t *p_config, uint32_t max_ms)
{  
  max_ms /= 16;
  for(uint16_t i = 0; i < max_ms; i++) {
    if (ADS1x1x_isConversionReady(p_config)) return true;
    delay(16);
  }
  return false;
}

/**************************************************************************/
/*!
    @brief  Read the ADC conversion result.
            The user must provide a valid pointer to a
            correctly filled ADS1x1x_config_t structure.
*/
/**************************************************************************/
int16_t ADS1x1x_read(ADS1x1x_config_t *p_config)
{
  // Read the conversion result.
  int16_t result = (int16_t)ADS1x1x_read_register(p_config->i2c_address,ADS1x1x_REG_POINTER_CONVERSION);
  // Adjust for ADC resolution if needed.
  if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
  {
    result >>= 4;
  }
  return result;
}

int32_t ADS1x1x_read_vol(ADS1x1x_config_t *p_config)
{
  const uint16_t fr[6]={6144, 4096, 2048, 1024, 512, 256};
  int32_t raw = ADS1x1x_read(p_config);
  int32_t ret = (fr[p_config->gain] * raw)/32768;
  return ret;
}


void ADS1x1x_set_threshold_lo(ADS1x1x_config_t *p_config, uint16_t value)
{
  if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
  {
    value <<= 4;
  }
  ADS1x1x_write_register(p_config->i2c_address,ADS1x1x_REG_POINTER_LO_THRESH,value);
}


void ADS1x1x_set_threshold_hi(ADS1x1x_config_t *p_config, uint16_t value)
{
  if (p_config->chip==ADS1013 || p_config->chip==ADS1014 || p_config->chip==ADS1015)
  {
    value <<= 4;
  }
  ADS1x1x_write_register(p_config->i2c_address,ADS1x1x_REG_POINTER_HI_THRESH,value);
}


void ADS1x1x_set_config_bitfield(ADS1x1x_config_t *p_config, uint16_t value, uint16_t mask)
{
  p_config->config &= ~mask;
  p_config->config |= (value & mask);
}


void ADS1x1x_set_os(ADS1x1x_config_t *p_config, ADS1x1x_os_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_OS_MASK);
}


void ADS1x1x_set_multiplexer(ADS1x1x_config_t *p_config, ADS1x1x_mux_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_MULTIPLEXER_MASK);
}


void ADS1x1x_set_pga(ADS1x1x_config_t *p_config, ADS1x1x_pga_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_PGA_MASK);
}


void ADS1x1x_set_mode(ADS1x1x_config_t *p_config, ADS1x1x_mode_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_MODE_MASK);
}


void ADS1x1x_set_data_rate(ADS1x1x_config_t *p_config, ADS1x1x_data_rate_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_DATA_RATE_MASK);
}


void ADS1x1x_set_comparator_mode(ADS1x1x_config_t *p_config, ADS1x1x_comparator_mode_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_MODE_MASK);
}


void ADS1x1x_set_comparator_polarity(ADS1x1x_config_t *p_config, ADS1x1x_comparator_polarity_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_POLARITY_MASK);
}


void ADS1x1x_set_comparator_latching(ADS1x1x_config_t *p_config, ADS1x1x_comparator_latching_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_LATCHING_MASK);
}


void ADS1x1x_set_comparator_queue(ADS1x1x_config_t *p_config, ADS1x1x_comparator_queue_t value)
{
  ADS1x1x_set_config_bitfield(p_config,(uint16_t)value,ADS1x1x_REG_CONFIG_COMPARATOR_QUEUE_MASK);
}
