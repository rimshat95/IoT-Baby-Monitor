import json
import os
import requests

DISCORD_WEBHOOK_URL = "https://discord.com/api/webhooks/1460103351462265091/-gZf6jUavH2mrYkjRc9zdOvv7Un-pUpegjqO4TR7HM3RjaR2MDInYjf7tiHjvTOfISFN"

def lambda_handler(event, context):
    print(f"Received event: {json.dumps(event)}")

    # Directly extract fields from event
    distance = event.get('distance', 'N/A')
    bed_status = event.get('bed_status', 'N/A')
    message_from_esp = event.get('message', 'N/A')
    device_id = event.get('deviceId', 'N/A')
    timestamp = event.get('timestamp', 'N/A')

    try:
        # Only send notifications when bed is detected as 'empty'
        if bed_status == 'empty':
            discord_message = {
                "content": f" **Baby Monitor Alert!** \n"
                           f"**Status:** {message_from_esp}\n"
                           f"**Distance:** {distance} cm\n"
                           f"**Device ID:** {device_id}\n"
                           f"**Timestamp:** {timestamp}"
            }
            
            response = requests.post(DISCORD_WEBHOOK_URL, json=discord_message)
            response.raise_for_status()
            
            print(f"Discord notification sent successfully. Status code: {response.status_code}")
            return {
                'statusCode': 200,
                'body': json.dumps('Discord notification sent!')
            }
        else:
            print(f"Bed status is '{bed_status}', no notification needed.")
            return {
                'statusCode': 200,
                'body': json.dumps('No notification sent for current status.')
            }

    except requests.exceptions.RequestException as e:
        print(f"Error sending Discord notification: {e}")
        return {
            'statusCode': 500,
            'body': json.dumps(f"Failed to send Discord notification: {e}")
        }
    except Exception as e:
        print(f"Error processing event: {e}")
        return {
            'statusCode': 500,
            'body': json.dumps(f"Error processing event: {e}")
        }
