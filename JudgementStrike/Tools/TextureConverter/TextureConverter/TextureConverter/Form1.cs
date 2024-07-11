using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace TextureConverter
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		private List<string> GetFils(string path)
		{
			var directories = Directory.GetDirectories(path);
			List<string> files = new List<string>();
			foreach (var dir in directories)
			{
				files.AddRange(GetFils(dir));
			}

			files.AddRange(Directory.GetFiles(path));
			return files;
		}
		private bool Convert(string path)
		{
			List<string> extensions = new List<string>();
			extensions.Add(".png");
			extensions.Add(".jpeg");
			extensions.Add(".jpg");
			extensions.Add(".bmp");
			extensions.Add(".tga");

			var ext = Path.GetExtension(path).ToLower();
			if (!extensions.Contains(ext))
			{
				return false;
			}
			var fmt = "DXT5";
			switch (ext)
			{
				case ".bmp":
				case ".jpeg":
					fmt = "DXT1";
					break;
			}

			Console.WriteLine($"Convert Texture:{path}");
			var dir = Path.GetDirectoryName(path);

			var info = new ProcessStartInfo();
			info.FileName = "bin/texconv.exe";
			info.ArgumentList.Add("-f");
			info.ArgumentList.Add(fmt);
			info.ArgumentList.Add("-o");
			info.ArgumentList.Add(dir);
			info.ArgumentList.Add(path);

			var process = Process.Start(info);
			process.WaitForExit();

			return true;
		}

		private void Form1_DragDrop(object sender, DragEventArgs e)
		{
			string[] files = (string[])e.Data.GetData(DataFormats.FileDrop, false);
			foreach (var fileName in files)
			{
				Console.WriteLine(fileName);
				if (Directory.Exists(fileName))
				{
					foreach (var path in GetFils(fileName))
					{
						Convert(path);
					}
				}
				else;
				{
					Convert(fileName);
				}
			}
		}

		private void Form1_DragEnter(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(DataFormats.FileDrop))
			{
				e.Effect = DragDropEffects.All;
			}
			else
			{
				e.Effect = DragDropEffects.None;
			}
		}
	}
}
