using idk;
using System.Collections.Generic;

namespace TestAndSeek
{
    public class LobbyManager
        : MonoBehavior
    {
        static List<Player> players = new List<Player>();

        public GameObject player0; // 0 is host player
        public GameObject player1;
        public GameObject player2;
        public GameObject player3;

        void Start()
        {
            ElectronNetwork.OnClientConnected += 
                (Player p) =>
                {
                    Debug.Log("Player " + p.ActorNumber + " connected!");
                    players.Add(p);
                    switch (p.ActorNumber)
                    {
                        case 0: player1.GetComponent<ElectronView>().TransferOwnership(p); break;
                        case 1: player2.GetComponent<ElectronView>().TransferOwnership(p); break;
                        case 2: player3.GetComponent<ElectronView>().TransferOwnership(p); break;
                    }
                };
            ElectronNetwork.OnClientDisconnected += 
                (Player p) => 
                { 
                    Debug.Log("Player " + p.ActorNumber + " disconnected!");
                    players.Remove(p);
                    switch (p.ActorNumber)
                    {
                        case 0: player1.GetComponent<ElectronView>().TransferOwnership(null); break;
                        case 1: player2.GetComponent<ElectronView>().TransferOwnership(null); break;
                        case 2: player3.GetComponent<ElectronView>().TransferOwnership(null); break;
                    }
                };

            ElectronNetwork.OnServerConnected += () => { Debug.Log("connected to server!"); };
            ElectronNetwork.OnServerDisconnected += () => { Debug.Log("disconnected from server!"); };

            foreach (var p in players)
            {
                switch (p.ActorNumber)
                {
                    case 0: player1.GetComponent<ElectronView>().TransferOwnership(p); break;
                    case 1: player2.GetComponent<ElectronView>().TransferOwnership(p); break;
                    case 2: player3.GetComponent<ElectronView>().TransferOwnership(p); break;
                }
            }
        }



        static IList<Player> GetPlayers()
        {
            return players.AsReadOnly();
        }
    }
}
