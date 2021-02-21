### A Pluto.jl notebook ###
# v0.12.20

using Markdown
using InteractiveUtils

# ╔═╡ d3bee05c-741f-11eb-0681-0966645e86b2
using Test, PlutoUI

# ╔═╡ d42c2810-73a2-11eb-2260-d3809a05056a
md"""
# Лабораторная работа №2. Блочное симметричное шифрование

## Цель работы

Изучение структуры и основных принципов работы современных алгоритмов блочного симметричного шифрования, приобретение навыков программной реализации блочных симметричных шифров.

## Задание

Вариант 7. Реализовать  систему симметричного блочного шифрования, позволяющую шифровать и дешифровать файл на диске с использованием блочного  шифра **TEA** в режиме  шифрования **CBC**.
"""

# ╔═╡ 5054c45e-741d-11eb-0329-d12b4c9d8132
struct CipherConfig
	key::UInt128
	num_rounds::UInt32
end

# ╔═╡ 89ed4be4-73ac-11eb-2008-b7556f5125d6
function tea_encrypt(config::CipherConfig, block::UInt64)::UInt64
	k1::UInt32, k2::UInt32, k3::UInt32, k4::UInt32 =
		(config.key >> 96 % UInt32,
		 config.key >> 64 % UInt32,
		 config.key >> 32 % UInt32,
		 config.key % UInt32)
	b1::UInt32, b2::UInt32 = (block >> 32 % UInt32, block % UInt32)
	
	delta::UInt32 = 0x9e3779b9
	sum::UInt32 = 0
	
	for i in 1:1:config.num_rounds
		sum += delta
		b1 += (b2 << 4 + k1) ⊻ (b2 + sum) ⊻ (b2 >> 5 + k2)
		b2 += (b1 << 4 + k3) ⊻ (b1 + sum) ⊻ (b1 >> 5 + k4)
	end
	
	UInt64(b1) << 32 | b2
end

# ╔═╡ f0c7e2ea-73ac-11eb-0593-09413f31d9e2
function tea_decrypt(config::CipherConfig, block::UInt64)::UInt64
	k1::UInt32, k2::UInt32, k3::UInt32, k4::UInt32 =
		(config.key >> 96 % UInt32,
		 config.key >> 64 % UInt32,
		 config.key >> 32 % UInt32,
		 config.key % UInt32)
	b1::UInt32, b2::UInt32 = (block >> 32 % UInt32, block % UInt32)
	
	log2_num_rounds = 31 - leading_zeros(config.num_rounds)
	
	delta::UInt32 = 0x9e3779b9
	sum::UInt32 = delta << log2_num_rounds
	
	for i in 1:1:config.num_rounds
		b2 -= (b1 << 4 + k3) ⊻ (b1 + sum) ⊻ (b1 >> 5 + k4)
		b1 -= (b2 << 4 + k1) ⊻ (b2 + sum) ⊻ (b2 >> 5 + k2)
		sum -= delta
	end
	
	UInt64(b1) << 32 | b2
end

# ╔═╡ b76fc4be-73ab-11eb-00b5-4796307a750c
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

# ╔═╡ eb172766-741d-11eb-2009-d7896f4e27a8
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

# ╔═╡ Cell order:
# ╠═d42c2810-73a2-11eb-2260-d3809a05056a
# ╠═d3bee05c-741f-11eb-0681-0966645e86b2
# ╠═5054c45e-741d-11eb-0329-d12b4c9d8132
# ╠═89ed4be4-73ac-11eb-2008-b7556f5125d6
# ╠═f0c7e2ea-73ac-11eb-0593-09413f31d9e2
# ╠═b76fc4be-73ab-11eb-00b5-4796307a750c
# ╠═eb172766-741d-11eb-2009-d7896f4e27a8
# ╠═07e9ecb0-73a7-11eb-2999-bb2422032477
