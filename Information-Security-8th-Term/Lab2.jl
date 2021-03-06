### A Pluto.jl notebook ###
# v0.12.21

using Markdown
using InteractiveUtils

# This Pluto notebook uses @bind for interactivity. When running this notebook outside of Pluto, the following 'mock version' of @bind gives bound variables a default value (instead of an error).
macro bind(def, element)
    quote
        local el = $(esc(element))
        global $(esc(def)) = Core.applicable(Base.get, el) ? Base.get(el) : missing
        el
    end
end

# ╔═╡ 89ed4be4-73ac-11eb-2008-b7556f5125d6
begin
	using Test, PlutoUI
	
	struct CipherConfig
		key::UInt128
		num_iterations::UInt32
	end
	
	function tea_encrypt(config::CipherConfig, block::UInt64)::UInt64
		k1::UInt32, k2::UInt32, k3::UInt32, k4::UInt32 =
			(config.key >> 96 % UInt32,
			 config.key >> 64 % UInt32,
			 config.key >> 32 % UInt32,
			 config.key % UInt32)
		# Split the block into the left and right halves
		l::UInt32, r::UInt32 = (block >> 32 % UInt32, block % UInt32)

		# Delta, derived from the golden ratio
		delta::UInt32 = 0x9e3779b9
		# Sum accumulates delta * current iteration in a variable
		# to avoid the multiplication on each iteration
		sum::UInt32 = 0

		for i in 1:1:config.num_iterations
			sum += delta
			l += (r << 4 + k1) ⊻ (r + sum) ⊻ (r >> 5 + k2)
			r += (l << 4 + k3) ⊻ (l + sum) ⊻ (l >> 5 + k4)
		end

		# Put the left and right halves back into a single block, now encrypted
		UInt64(l) << 32 | r
	end
	
	function tea_decrypt(config::CipherConfig, block::UInt64)::UInt64
		k1::UInt32, k2::UInt32, k3::UInt32, k4::UInt32 =
			(config.key >> 96 % UInt32,
			 config.key >> 64 % UInt32,
			 config.key >> 32 % UInt32,
			 config.key % UInt32)
		l::UInt32, r::UInt32 = (block >> 32 % UInt32, block % UInt32)

		delta::UInt32 = 0x9e3779b9
		# To reverse the addition of delta, the value on each iteration is
		# computed as `delta * (num iterations - current iteration)`
		log2_num_rounds = 31 - leading_zeros(config.num_iterations)
		sum::UInt32 = delta << log2_num_rounds

		for i in 1:1:config.num_iterations
			r -= (l << 4 + k3) ⊻ (l + sum) ⊻ (l >> 5 + k4)
			l -= (r << 4 + k1) ⊻ (r + sum) ⊻ (r >> 5 + k2)
			sum -= delta
		end

		UInt64(l) << 32 | r
	end
end

# ╔═╡ d42c2810-73a2-11eb-2260-d3809a05056a
md"""
# Лабораторная работа №2. Блочное симметричное шифрование

## Цель работы

Изучение структуры и основных принципов работы современных алгоритмов блочного симметричного шифрования, приобретение навыков программной реализации блочных симметричных шифров.

## Задание

Вариант 7. Реализовать  систему симметричного блочного шифрования, позволяющую шифровать и дешифровать файл на диске с использованием блочного шифра **TEA** в режиме  шифрования **CBC**.

## Описание шифра

**TEA** является блочным шифром на основе сети Фейстеля с 64-битными блоками и 128-битным ключом.

При шифровании блок и ключ разделяются на две половины. На одной итерации шифра считается два раунда. Сначала правая половина блока изменяется функцией от первой половины ключа, результат складывается по модулю 2^32 с левой половиной блока. Затем левая половина блока изменяется функцией от второй половины ключа, результат складывается с правой половиной блока. Как можно заметить, отличием **TEA** от других шифров на основе сети Фейстеля является использование сложения по модулю 2^32, а не 2, для аккумуляции результата.

В функции преобразования используются битовые сдвиги (блок перестановок) и сложение с дополнительной константой, выведенной из золотого сечения (0x9e3779b9). Константа домножается на индекс итерации, что способствует предотвращению определенного класса атак.

## Описание режима шифрования

Режим шифрования описывает то, как блочный шифр применяется к исходным данным размером более одного блока.

В режиме **CBC** каждый последующий блок _перед шифрованием_ складывается по модулю 2 с предыдущим _зашифрованным_ блоком. При обработке первого блока должна использоваться уникальная последовательность (**IV** — _initialization vector_, вектор инициализации).

При дешифровании, _дешифрованный блок_ складывается по модулю 2 с предыдущим _зашифрованным блоком_ (IV при обработке первого блока). Таким образом, при неправильном указании IV будет утерян только первый блок данных.
"""

# ╔═╡ 5af1430e-7929-11eb-2226-effab70fc003
md"""
## Интерфейс взаимодействия с программой шифрования

Режим: $(@bind uimode Select(["Не выбран", "Зашифровать", "Расшифровать"]))

Число итераций: $(@bind uiiters Select(["8", "16", "32", "64"], default="32"))

Ключ (0x...): $(@bind uikey TextField())

Вектор инициализации (0x...): $(@bind uiiv TextField())

Файл: $(@bind uifile FilePicker())
"""

# ╔═╡ 05819c9e-7a59-11eb-2258-6df8c29b9b7c
begin	
	function cbc_encrypt(config::CipherConfig, iv::UInt64, i::IO, o::IO)
		xor_with = iv

		while !eof(i)
			block = read(i, UInt64)
			block ⊻= xor_with
			enc_block = tea_encrypt(config, block)
			write(o, enc_block)
			xor_with = enc_block
		end
	end
	
	function cbc_decrypt(config::CipherConfig, iv::UInt64, i::IO, o::IO)
		xor_with = iv

		while !eof(i)
			enc_block = read(i, UInt64)
			block = tea_decrypt(config, enc_block)
			block ⊻= xor_with
			write(o, block)
			xor_with = enc_block
		end
	end
end

# ╔═╡ 8ef45376-7929-11eb-1730-21631472630d
begin
	if uimode == "Не выбран"
		md"""
		## Результат работы программы
		Не выбран режим работы.
		"""
	elseif isnothing(tryparse(UInt128, uikey)) || isnothing(tryparse(UInt64, uiiv))
		md"""
		## Результат работы программы
		Неверно введены ключ и вектор инициализации.
		"""
	elseif isempty(uifile["data"])
		md"""
		## Результат работы программы
		Не выбран исходный файл.
		"""
	else
		input = uifile["data"]
		if length(input) % 8 != 0
			padding = repeat([UInt8(0)], 8 - length(input) % 8)
			input = [input; padding]
		end
		input = IOBuffer(input)
		output = IOBuffer()
		
		config = CipherConfig(parse(UInt128, uikey), parse(UInt32, uiiters))
		iv = parse(UInt64, uiiv)
		
		f = uimode == "Зашифровать" ? cbc_encrypt : cbc_decrypt
		
		try
			f(config, iv, input, output)

			res_button = DownloadButton(output.data, uifile["name"])

			data_size = length(uifile["data"]) % 8 == 0 ?
				"$(length(uifile["data"])) байт" :
				"$(length(uifile["data"])) байта информации + $(8 - length(uifile["data"]) % 8) заполняющих байт"
			
			md"""
			## Результат работы программы
			Размер данных: $(data_size)

			Результат: $(res_button)
			"""
		catch e
			md"""
			## Результат работы программы
			Ошибка: $(e)
			"""
		end
	end
end

# ╔═╡ 218c9b0e-7929-11eb-347a-e7ab91cc1e45
md"""
## Тестирование алгоритма
"""

# ╔═╡ 07e9ecb0-73a7-11eb-2999-bb2422032477
with_terminal() do
	@testset "encryption and decryption" begin
		conf = CipherConfig(UInt128(0x666), UInt32(32))
		iv = UInt64(0x313)
		
		orgbuf = IOBuffer(reinterpret(UInt8, [1,2,3,4,5,6]))
		encbuf = IOBuffer()
		decbuf = IOBuffer()
		
		cbc_encrypt(conf, iv, orgbuf, encbuf)
		seekstart(encbuf)
		cbc_decrypt(conf, iv, encbuf, decbuf)
		
		@test reinterpret(UInt64, encbuf.data) == [
			0x61137e4fdb64c60b, 0xda3ca992451598ef, 0x9c3928cfffa5794d,
			0x6110330d7c396aae, 0xae22265919967777, 0xfb0e9c9dc7055110
		]
		@test orgbuf.data == decbuf.data
	end
end

# ╔═╡ 89349308-7a58-11eb-3122-abc8156ebe8a
md"""
## Вывод

В ходе выполнения работы была рассмотрена теоретическая основа блочного шифра TEA и режима шифрования CBC. Реализованы алгоритмы шифрования и дешифрования.
"""

# ╔═╡ Cell order:
# ╟─d42c2810-73a2-11eb-2260-d3809a05056a
# ╟─5af1430e-7929-11eb-2226-effab70fc003
# ╟─8ef45376-7929-11eb-1730-21631472630d
# ╠═89ed4be4-73ac-11eb-2008-b7556f5125d6
# ╠═05819c9e-7a59-11eb-2258-6df8c29b9b7c
# ╟─218c9b0e-7929-11eb-347a-e7ab91cc1e45
# ╠═07e9ecb0-73a7-11eb-2999-bb2422032477
# ╟─89349308-7a58-11eb-3122-abc8156ebe8a
