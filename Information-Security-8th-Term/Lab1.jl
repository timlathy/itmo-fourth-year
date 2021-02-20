### A Pluto.jl notebook ###
# v0.12.20

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

# ╔═╡ 4fa0562e-721d-11eb-3833-cd829ef088d0
using Test, Logging, PlutoUI

# ╔═╡ 0a7cf952-73a1-11eb-3f5d-aff7400016c2
md"""
# Лабораторная работа №1. Основы шифрования данных

## Цель работы

Изучение основных принципов шифрования информации, знакомство с широко известными алгоритмами шифрования, приобретение навыков их программной реализации.

## Задание

Вариант 7. Реализовать в программе шифрование и дешифрацию файла с использованием перестановочного шифра с ключевым словом. Ключевое слово вводится.
"""

# ╔═╡ 17e5bb2a-72b0-11eb-24be-8b92195c12ec
md"""
## Интерфейс взаимодействия с программой шифрования

Режим: $(@bind uimode Select(["Не выбран", "Зашифровать", "Расшифровать"]))

Ключ: $(@bind uikey TextField())

Удалить пробелы и знаки пунктуации: $(@bind uinopunct CheckBox(default=true))

Файл: $(@bind uifile FilePicker())
"""

# ╔═╡ 1e0bb172-73a2-11eb-051e-5dd4c7e4a536
begin
	function encrypt(text::String, key::String)
		keychars::Array{Char,1} = collect(key)
		textchars::Array{Char,1} = collect(text)

		numcols = length(keychars)
		# Text is arranged into `numcols` columns. If the number of characters
		# in the text is not divisible by the number of columns, we need to
		# insert padding symbols (the first symbol in the text in this case).
		modchars = length(textchars) % numcols
		if modchars > 0
			padding = repeat([textchars[1]], numcols - modchars)
			append!(textchars, padding)
		end

		T = permutedims(reshape(textchars, numcols, :))
		p = sortperm(keychars)

		ciphertextchars = reshape(T[:, p], 1, :)
		join(ciphertextchars)
	end
	
	function decrypt(ciphertext::String, key::String)
		keychars::Array{Char,1} = collect(key)
		ciphertextchars::Array{Char,1} = collect(ciphertext)

		numcols = length(keychars)

		if length(ciphertext) % numcols > 0
			throw(ArgumentError("Длина зашифрованного текста не кратна длине ключа"))
		end

		T = reshape(ciphertextchars, :, numcols)
		revkeyperm = sortperm(sortperm(keychars))

		textchars = reshape(permutedims(T[:, revkeyperm]), 1, :)
		join(textchars)
	end
	
	md"## Исходный код реализации"
end

# ╔═╡ b13fe00c-7399-11eb-3f10-81af4e8318df
begin
	if uimode == "Не выбран"
		md"""
		## Результат работы программы
		Не выбран режим работы.
		"""
	elseif isempty(uikey)
		md"""
		## Результат работы программы
		Не введен ключ.
		"""
	elseif isempty(uifile["data"])
		md"""
		## Результат работы программы
		Не выбран исходный файл.
		"""
	else
		input = read(IOBuffer(uifile["data"]), String)
		f = uimode == "Зашифровать" ? encrypt : decrypt
		
		if uinopunct
			input = replace(input, r"[[:punct:][:space:]]" => "")
		end
		
		try
			res = f(input, uikey)

			res_button = DownloadButton(res, "result.txt")

			md"""
			## Результат работы программы
			Длина ключа: $(length(uikey))

			Длина входного текста: $(length(input))

			Длина результата: $(length(res))

			Результат: $(res_button)
			"""
		catch e
			md"""
			## Результат работы программы
			Ошибка: $(e.msg)
			"""
		end
	end
end

# ╔═╡ 43e0bd52-73a2-11eb-3248-2987c2a36299
md"## Тестирование алгоритма"

# ╔═╡ fa76e7b2-721c-11eb-13b8-471a355ee345
with_terminal() do
	@testset "encryption" begin
		# arrange 12345678 into 4 columns:
		# a b c d
		# e f g h
		# next, rearrange the columns as 4321 (DCBA -> ABCD):
		# d c b a
		# h g f e
		# finally, flatten the columns into one in column-major order:
		# d h c g b f a e
		@test encrypt("abcdefgh", "DCBA") == "dhcgbfae"
		@test decrypt("dhcgbfae", "DCBA") == "abcdefgh"
	end

	@testset "key mismatch" begin
		@test_throws ArgumentError decrypt("1234", "123")
	end
end

# ╔═╡ Cell order:
# ╟─0a7cf952-73a1-11eb-3f5d-aff7400016c2
# ╟─4fa0562e-721d-11eb-3833-cd829ef088d0
# ╟─17e5bb2a-72b0-11eb-24be-8b92195c12ec
# ╟─b13fe00c-7399-11eb-3f10-81af4e8318df
# ╠═1e0bb172-73a2-11eb-051e-5dd4c7e4a536
# ╟─43e0bd52-73a2-11eb-3248-2987c2a36299
# ╠═fa76e7b2-721c-11eb-13b8-471a355ee345
