from PIL import Image, ImageDraw, ImageFont

def create_dev_home_icon(size):
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Outer rounded rectangle background (Microsoft blue/purple gradient aesthetic)
    margin = int(size * 0.05)
    rect = [margin, margin, size - margin, size - margin]
    radius = int(size * 0.2)

    # Draw dark Fluent rounded background
    draw.rounded_rectangle(rect, radius=radius, fill=(15, 23, 42, 255), outline=(56, 189, 248, 255), width=max(1, int(size * 0.03)))

    # Inside content: Visual Studio / Dev Home "< />" or Grid Dashboard icon
    # Top bar (Window header representation)
    bar_h = int(size * 0.15)
    draw.rectangle([margin + radius//2, margin + int(size * 0.1), size - margin - radius//2, margin + int(size * 0.22)], fill=(30, 41, 59, 255))

    # Grid elements (Widgets)
    w = (size - 2 * margin)
    padding = int(w * 0.08)
    cell_w = (w - 3 * padding) // 2
    top_y = margin + bar_h + int(size * 0.1)
    cell_h = int(size * 0.22)

    # Widget 1 (Blue gradient Accent)
    draw.rounded_rectangle([margin + padding, top_y, margin + padding + cell_w, top_y + cell_h], radius=int(size*0.05), fill=(14, 165, 233, 255))
    # Widget 2 (Purple Accent)
    draw.rounded_rectangle([margin + 2*padding + cell_w, top_y, size - margin - padding, top_y + cell_h], radius=int(size*0.05), fill=(139, 92, 246, 255))

    # Bottom Widget (Green / Teal Accent)
    top_y2 = top_y + cell_h + padding
    draw.rounded_rectangle([margin + padding, top_y2, size - margin - padding, size - margin - padding], radius=int(size*0.05), fill=(16, 185, 129, 255))

    return img

images = [create_dev_home_icon(s) for s in [16, 32, 48, 64, 128, 256]]
images[0].save('assets/app.ico', format='ICO', sizes=[(s, s) for s in [16, 32, 48, 64, 128, 256]], append_images=images[1:])
print("Saved assets/app.ico successfully")
