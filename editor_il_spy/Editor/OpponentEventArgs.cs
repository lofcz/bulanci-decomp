using System;

namespace Editor;

public sealed class OpponentEventArgs : EventArgs
{
	private Opponent _opponent;

	public Opponent Opponent => _opponent;

	public OpponentEventArgs(Opponent opponent)
	{
		_opponent = opponent;
	}
}
