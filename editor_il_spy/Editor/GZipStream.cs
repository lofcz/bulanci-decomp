using System;
using System.Collections.Generic;
using System.IO;
using zlib;

namespace Editor;

public sealed class GZipStream : Stream
{
	private bool m_bWriting;

	private long m_lPos;

	private long m_lStart;

	private long m_lSize;

	private const int m_nBlockSize = 32768;

	private Stream m_stream;

	private List<long> m_chunks;

	private byte[] m_buffer;

	private int m_nBufLoaded;

	private int m_nBufSize;

	public const uint Magic = 1346984519u;

	public override bool CanRead => !m_bWriting;

	public override bool CanSeek => !m_bWriting;

	public override bool CanWrite => m_bWriting;

	public override long Length => m_lSize;

	public override long Position
	{
		get
		{
			return m_lPos;
		}
		set
		{
			Seek(value, SeekOrigin.Begin);
		}
	}

	public GZipStream()
	{
	}

	public GZipStream(Stream stream, bool create)
	{
		Open(stream, create);
	}

	~GZipStream()
	{
		CloseInt();
	}

	public override void Flush()
	{
	}

	public override long Seek(long offset, SeekOrigin origin)
	{
		if (m_bWriting)
		{
			throw new InvalidOperationException();
		}
		long lPos = -1L;
		switch (origin)
		{
		case SeekOrigin.Begin:
			lPos = offset;
			break;
		case SeekOrigin.Current:
			lPos = m_lPos + offset;
			break;
		case SeekOrigin.End:
			lPos = m_lSize - offset;
			break;
		}
		if (m_lPos < 0 || m_lPos > m_lSize)
		{
			throw new ArgumentOutOfRangeException("offset");
		}
		m_lPos = lPos;
		return m_lPos;
	}

	public override void SetLength(long value)
	{
		throw new NotSupportedException();
	}

	public override int Read(byte[] buffer, int offset, int count)
	{
		if (m_bWriting)
		{
			throw new InvalidOperationException();
		}
		if (offset < 0 || offset >= buffer.Length)
		{
			throw new ArgumentOutOfRangeException("offset");
		}
		if (count < 0 || offset + count > buffer.Length)
		{
			throw new ArgumentOutOfRangeException("count");
		}
		int num = 0;
		while (count > 0 && m_lPos < m_lSize)
		{
			int num2 = (int)(m_lPos % 32768);
			ReadBufferAt((int)(m_lPos / 32768));
			int num3 = Math.Min(count, m_nBufSize - num2);
			Array.Copy(m_buffer, num2, buffer, offset, num3);
			m_lPos += num3;
			offset += num3;
			count -= num3;
			num += num3;
		}
		return num;
	}

	public override void Write(byte[] buffer, int offset, int count)
	{
		if (!m_bWriting)
		{
			throw new InvalidOperationException();
		}
		if (offset < 0 || offset >= buffer.Length)
		{
			throw new ArgumentOutOfRangeException("offset");
		}
		if (count < 0 || offset + count > buffer.Length)
		{
			throw new ArgumentOutOfRangeException("count");
		}
		while (count > 0)
		{
			if (m_nBufSize == 32768)
			{
				WriteBuffer();
			}
			int num = Math.Min(count, 32768 - m_nBufSize);
			Array.Copy(buffer, offset, m_buffer, m_nBufSize, num);
			m_lPos += num;
			m_lSize += num;
			m_nBufSize += num;
			offset += num;
			count -= num;
		}
	}

	public override void Close()
	{
		CloseInt();
		base.Close();
	}

	public void Open(Stream stream, bool create)
	{
		CloseInt();
		m_bWriting = create;
		m_lStart = stream.Position;
		m_chunks = new List<long>();
		m_nBufLoaded = -1;
		m_lPos = 0L;
		m_lSize = 0L;
		if (m_bWriting)
		{
			stream.SetLength(m_lStart);
			BinaryWriter binaryWriter = new BinaryWriter(stream);
			binaryWriter.Write(1346984519u);
			binaryWriter.Write(0L);
			m_chunks.Capacity = 4096;
			m_chunks.Add(12L);
			m_buffer = new byte[32768];
		}
		else
		{
			BinaryReader binaryReader = new BinaryReader(stream);
			if (binaryReader.ReadUInt32() != 1346984519)
			{
				throw new BadImageFormatException();
			}
			long num = binaryReader.ReadInt64();
			stream.Seek(m_lStart + num, SeekOrigin.Begin);
			m_lSize = binaryReader.ReadInt64();
			int num2 = (int)((m_lSize + 32768 - 1) / 32768 + 2);
			m_chunks.Capacity = num2;
			m_buffer = new byte[32768];
			int num3 = binaryReader.ReadInt32();
			byte[] array = new byte[num3];
			stream.Read(array, 0, num3);
			int destLen = 8 * num2;
			byte[] array2 = new byte[destLen];
			if (Decompress(array2, ref destLen, array, num3) != 0)
			{
				throw new BadImageFormatException();
			}
			using MemoryStream input = new MemoryStream(array2, writable: false);
			using BinaryReader binaryReader2 = new BinaryReader(input);
			for (int i = 1; i < num2; i++)
			{
				m_chunks.Add(binaryReader2.ReadInt64());
			}
			m_chunks.Add(m_chunks[m_chunks.Count - 1]);
		}
		m_stream = stream;
	}

	private void CloseInt()
	{
		if (m_stream != null && m_bWriting)
		{
			WriteBuffer();
			long num = m_chunks[m_chunks.Count - 1];
			BinaryWriter binaryWriter = new BinaryWriter(m_stream);
			m_stream.Seek(m_lStart + 4, SeekOrigin.Begin);
			binaryWriter.Write(num);
			m_stream.Seek(m_lStart + num, SeekOrigin.Begin);
			binaryWriter.Write(m_lSize);
			int destLen = (int)((double)(m_chunks.Count * 8) * 1.2 + 20.0);
			using MemoryStream memoryStream = new MemoryStream();
			BinaryWriter bw = new BinaryWriter(memoryStream);
			try
			{
				m_chunks.ForEach(delegate(long x)
				{
					bw.Write(x);
				});
				byte[] array = memoryStream.ToArray();
				byte[] array2 = new byte[destLen];
				if (Compress(array2, ref destLen, array, array.Length) != 0)
				{
					throw new BadImageFormatException();
				}
				binaryWriter.Write(destLen);
				binaryWriter.Write(array2, 0, destLen);
			}
			finally
			{
				if (bw != null)
				{
					((IDisposable)bw).Dispose();
				}
			}
		}
		if (m_stream != null)
		{
			m_stream.Close();
		}
		m_stream = null;
		m_buffer = null;
	}

	private void ReadBufferAt(int n)
	{
		if (n == m_nBufLoaded)
		{
			return;
		}
		int num = (int)(m_chunks[n + 1] - m_chunks[n]);
		if (num <= 0 || num > 32768)
		{
			throw new BadImageFormatException();
		}
		m_stream.Seek(m_lStart + m_chunks[n], SeekOrigin.Begin);
		if (num < 32768)
		{
			byte[] array = new byte[num];
			m_stream.Read(array, 0, num);
			int destLen = 32768;
			if (Decompress(m_buffer, ref destLen, array, num) != 0)
			{
				throw new BadImageFormatException();
			}
			m_nBufSize = destLen;
		}
		else
		{
			m_stream.Read(m_buffer, 0, num);
			m_nBufSize = num;
		}
		m_nBufLoaded = n;
	}

	private void WriteBuffer()
	{
		int index = m_chunks.Count - 1;
		m_stream.Seek(m_lStart + m_chunks[index], SeekOrigin.Begin);
		if (m_nBufSize > 0)
		{
			int destLen = 39341;
			byte[] array = new byte[destLen];
			if (Compress(array, ref destLen, m_buffer, m_nBufSize) != 0)
			{
				throw new BadImageFormatException();
			}
			if (destLen < m_nBufSize)
			{
				m_stream.Write(array, 0, destLen);
			}
			else
			{
				m_stream.Write(m_buffer, 0, m_nBufSize);
			}
		}
		m_chunks.Add(m_stream.Position - m_lStart);
		m_nBufSize = 0;
	}

	private static int Compress(byte[] dest, ref int destLen, byte[] source, int sourceLen)
	{
		ZStream zStream = new ZStream();
		zStream.next_in = source;
		zStream.avail_in = sourceLen;
		zStream.next_out = dest;
		zStream.avail_out = destLen;
		int num = zStream.deflateInit(9);
		if (num == 0)
		{
			num = zStream.deflate(4);
			if (num != 1)
			{
				zStream.deflateEnd();
				if (num == 0)
				{
					num = -5;
				}
			}
			else
			{
				destLen = (int)zStream.total_out;
				num = zStream.deflateEnd();
			}
		}
		zStream.free();
		return num;
	}

	private static int Decompress(byte[] dest, ref int destLen, byte[] source, int sourceLen)
	{
		ZStream zStream = new ZStream();
		zStream.next_in = source;
		zStream.avail_in = sourceLen;
		zStream.next_out = dest;
		zStream.avail_out = destLen;
		int num = zStream.inflateInit();
		if (num == 0)
		{
			num = zStream.inflate(4);
			if (num != 1)
			{
				zStream.inflateEnd();
				if (num == 0)
				{
					num = -5;
				}
			}
			else
			{
				destLen = (int)zStream.total_out;
				num = zStream.inflateEnd();
			}
		}
		zStream.free();
		return num;
	}
}
