#pragma once
#include "Common.h"

namespace mtbin
{
	/// <summary>
	/// <para>Byte配列のストリーム</para>
	/// </summary>
	class MemoryStream
	{
	public:
		using SeekPoint = size_t;

		enum struct SeekDir : SeekPoint
		{
			Head,  // 一番頭
		};

	public:
		MemoryStream(mtbin::Byte* _pBuffer, const size_t& _bufferSize);
		virtual ~MemoryStream();

		/// <summary>
		/// メモリストリームに任意の型を書き込む
		/// </summary>
		/// <typeparam name="T">任意の型</typeparam>
		/// <param name="_value">書き込むオブジェクト</param>
		template<typename T>
		void Write(T _value);

		/// <summary>
		/// メモリストリームに任意の型の配列を書き込む
		/// </summary>
		/// <param name="_pArray">配列のポインタ</param>
		/// <param name="_arrayLength">要素数</param>
		template<typename T>
		void Write(T* _pArray, const int& _arrayLength);

		/// <summary>
		/// メモリストリームから任意の型サイズ分読み取る
		/// </summary>
		/// <typeparam name="T">任意の型</typeparam>
		/// <returns>読み取ったオブジェクト</returns>
		template<typename T>
		T Read();

		/// <summary>
		/// メモリストリームから任意の型の配列を読み取る
		/// </summary>
		/// <param name="_pWriteBuffer">読み取って書き込む配列のポインタ</param>
		/// <param name="_writeBufferLength">読み取って書き込む配列の要素数</param>
		/// <param name="_arrayLength">要素数</param>
		template<typename T>
		void Read(T* _pWriteBuffer, const int& _writeBufferLength, const int& _arrayLength);

		/// <summary>
		/// 読み書きする場所を移動する
		/// </summary>
		/// <param name="_point">移動する場所(直)</param>
		void Seek(SeekPoint _point);
		/// <summary>
		/// 読み書きする場所を移動する
		/// </summary>
		/// <param name="_dir">絶対的な場所指定</param>
		void Seek(SeekDir _dir);

		/// <summary>
		/// 現在の読み書き位置を取得
		/// </summary>
		/// <returns></returns>
		inline size_t Current() const { return currentIndex; }

	private:
		const size_t BUFFER_SIZE;  // バッファ配列のサイズ
		mtbin::Byte* pBuffer_;  // 読み書きするバッファ配列
		size_t currentIndex;  // 現在のインデックス
	};

	template<typename T>
	inline void MemoryStream::Write(T _value)
	{
		assert((currentIndex + sizeof(T)) < BUFFER_SIZE  // 書き込んでもオーバーランしない
			&& "buffer over run @mtbin::MemoryStream::Write");

		void* dist = &(pBuffer_[currentIndex]);
		void* src = reinterpret_cast<void*>(&_value);
		size_t size = sizeof(T);
		// 書き込む
		::memcpy(dist, src, size);
		currentIndex += sizeof(T);  // サイズ分進める
	}

	template<typename T>
	inline void MemoryStream::Write(T* _pArray, const int& _arrayLength)
	{
		size_t size{ sizeof(T) * _arrayLength };  // 書き込むサイズ
		assert((currentIndex + size) <= BUFFER_SIZE  // 書き込んでもオーバーランしない
			&& "buffer over run @mtbin::MemoryStream::Write");

		// 書き込む
		::memcpy(
			&(pBuffer_[currentIndex]),
			reinterpret_cast<mtbin::Byte*>(_pArray),
			size);
		currentIndex += size;  // サイズ分進める
	}

	template<typename T>
	inline T MemoryStream::Read()
	{
		assert((currentIndex + sizeof(T)) <= BUFFER_SIZE  // 読み込んでもアンダーランしない
			&& "buffer under run @mtbin::MemoryStream::Read");

		T pickBuffer{};  // 取り出し用バッファ用意
		::memcpy(&pickBuffer, &(pBuffer_[currentIndex]), sizeof(T));  // 取り出す
		currentIndex += sizeof(T);  // サイズ分進める

		return pickBuffer;
	}

	template<typename T>
	inline void MemoryStream::Read(T* _pWriteBuffer, const int& _writeBufferLength, const int& _arrayLength)
	{
		assert(_pWriteBuffer != nullptr  // 書き込み先はnullptrではない
			&& "write buffer is nullptr @mtbin::MemoryStream::Read");

		assert(_writeBufferLength >= _arrayLength  // 書き込み先に十分な容量あり
			&& "write buffer under run @mtbin::MemoryStream::Read");

		size_t size{ sizeof(T) * _arrayLength };  // 読み込むサイズ
		assert((currentIndex + size) <= BUFFER_SIZE  // 読み込んでもアンダーランしない
			&& "buffer under run @mtbin::MemoryStream::Read");

		::memcpy(_pWriteBuffer, &(pBuffer_[currentIndex]), size);  // 取り出す
		currentIndex += size;  // サイズ分進める
	}
}
