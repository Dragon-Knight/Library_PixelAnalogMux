#pragma once
#include <inttypes.h>
#include <EasyPinD.h>

template <uint8_t _count = 0> 
class AnalogMux
{
	static constexpr uint16_t _tick_time = 25;
	
	using func_adc_req_t = uint16_t (*)(uint8_t address);
	using func_result_t = void (*)(uint8_t address, uint16_t value);
	
	public:
		
		template <typename... DPinTs> 
		AnalogMux(func_adc_req_t adc_req, func_result_t result, DPinTs... dpins) : 
			_callback_adc_req(adc_req), 
			_callback_result(result),
			_digital{ EasyPinD(dpins.Port, {dpins.Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH})... }
		{
			static_assert(sizeof...(dpins) == _count, "Incorrect number of digital pins provided");
		}

		void Init()
		{
			return;
		}

		uint16_t Get(uint8_t address)
		{
			for(uint8_t i = 0; i < _count; ++i)
			{
				if(address & (1 << i)){
					_digital[i].On();
				} else {
					_digital[i].Off();
				}
			}
			
			return _callback_adc_req(address);
		}

		void Processing(uint32_t &time)
		{
			if(time - _last_tick < _tick_time) return;
			_last_tick = time;
			
			uint16_t adc_raw;
			for(uint8_t addr = 0; addr < (1 << _count); ++addr)
			{
				adc_raw = Get(addr);
				_callback_result(addr, adc_raw);
			}
			
			return;
		}

	private:
		
		func_adc_req_t _callback_adc_req = nullptr;
		func_result_t _callback_result = nullptr;
		
		EasyPinD _digital[_count];
		
		uint32_t _last_tick = 0;
};
